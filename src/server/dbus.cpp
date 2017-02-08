#include "dbus.h"

#include <iostream>

#include "../event/debugstuff.h"
#include "../event/thm_signal.h"
#include "../event/req_procedure_call.h"
#include "../event/req_shell_command.h"
#include "../state/thm.h"
#include "../satellite.h"
#include "../util.h"


namespace horst {

Satellite* globalsat;

/*
 * callback method used by libuv to notify when
 * dbus messages can be processed
 */
static void on_dbus_ready(uv_poll_t *handle,
                          int /*status*/,
                          int events) {

	std::cout << "[dbus] bus is ready for some events: "
	          << events << std::endl;

	DBusConnection *connection = (DBusConnection *)handle->data;

	sd_bus *bus = connection->get_bus();

	// let dbus handle the requests available
	while (true) {
		int r = sd_bus_process(bus, nullptr);
		if (r < 0) {
			std::cout << "Failed to process dbus:  "
			          << strerror(-r) << std::endl;
			break;
		}
		else if (r > 0) {
			// try to process another request!
			continue;
		}
		else {
			// no more progress, wait for the next callback.
			break;
		}
	}

	// update the events we watch for on the socket.
	connection->update_events();
}


DBusConnection::DBusConnection(Satellite *sat)
	:
	satellite{sat},
	loop{sat->get_loop()},
	bus{nullptr},
	bus_slot{nullptr} {}


DBusConnection::~DBusConnection() {}


void DBusConnection::update_events() {
	sd_bus *bus = this->get_bus();

	// prepare the callback for calling us the next time.
	int new_events = util::poll_to_libuv_events(
		sd_bus_get_events(bus)
	);

	uint64_t usec;
	int r = sd_bus_get_timeout(bus, &usec);

	if (not r) {
		// if the timer is running already, it is stopped automatically
		// inside uv_timer_start.
		uv_timer_start(
			&this->timer,
			[] (uv_timer_t *handle) {
				// yes, handle is not a poll_t, but
				// we just care for its -> data member anyway.
				on_dbus_ready((uv_poll_t *)handle, 0, 0);
			},
			usec / 1000, // time in milliseconds, sd_bus provides µseconds
			0            // don't repeat
		);
	}

	// always watch for disconnects:
	new_events |= UV_DISCONNECT;

	// activate the socket watching,
	// and if active, invoke the callback function
	uv_poll_start(&this->connection,
	              new_events,
	              &on_dbus_ready);
}


static int dbus_exec(sd_bus_message *m,
                     void *userdata,
                     sd_bus_error * /*ret_error*/) {

	DBusConnection *this_ = (DBusConnection *)userdata;
	const char *command;
	int r = sd_bus_message_read(m, "s", &command);
	if (r < 0) {
		std::cout << "[dbus] exec() failed to parse parameters: "
		          << strerror(-r) << std::endl;
		return r;
	}

	std::cout << "[dbus] exec() command: " << command << std::endl;

	auto req = std::make_shared<ShellCommandReq>(command);
	this_->get_sat()->on_event(std::move(req));

	// TODO: maybe do req->call_on_complete([]{real_dbus_return()})

	return sd_bus_reply_method_return(m, "x", 0);
}


static int dbus_run(sd_bus_message *m,
                    void *userdata,
                    sd_bus_error * /*ret_error*/) {

	DBusConnection *this_ = (DBusConnection *)userdata;
	const char *name;
	int r = sd_bus_message_read(m, "s", &name);
	if (r < 0) {
		std::cout << "[dbus] run() failed to parse parameters: "
		          << strerror(-r) << std::endl;
		return r;
	}

	std::cout << "[dbus] run() procedure: " << name << std::endl;

	auto req = std::make_shared<ProcedureCallReq>(name);
	this_->get_sat()->on_event(std::move(req));

	return sd_bus_reply_method_return(m, "x", 0);
}


static int dbus_set(sd_bus_message *m,
                    void *userdata,
                    sd_bus_error * /*ret_error*/) {

	DBusConnection *this_ = (DBusConnection *)userdata;
	const char *name;
	int r = sd_bus_message_read(m, "s", &name);
	if (r < 0) {
		std::cout << "[dbus] set() failed to parse parameters: "
		          << strerror(-r) << std::endl;
		return r;
	}

	std::cout << "[dbus] set() debug: " << name << std::endl;

	auto req = std::make_shared<DebugStuff>(name);
	this_->get_sat()->on_event(std::move(req));

	return sd_bus_reply_method_return(m, "x", 0);
}


static const sd_bus_vtable horst_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("run", "s", "x", dbus_run, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("exec", "s", "x", dbus_exec, SD_BUS_VTABLE_UNPRIVILEGED),
	// debugging function: remove it!
	SD_BUS_METHOD("set", "s", "x", dbus_set, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_SIGNAL("actionDone", "bt", 0),
	SD_BUS_VTABLE_END
};


int DBusConnection::connect() {
	// open bus: system or user
	int r = sd_bus_open_user(&this->bus);

	if (r < 0) {
		std::cout << "[dbus] Failed to connect to system/user bus:  "
		          << strerror(-r) << std::endl;
		goto clean_return;
	}

	// install the horst object
	r = sd_bus_add_object_vtable(
		this->bus,
		&this->bus_slot,
		"/moveii/horst",  // object path
		"moveii.horst",   // interface name
		horst_vtable,
		this              // this is the userdata that'll be passed
		                  // to the dbus methods
	);

	if (r < 0) {
		std::cout << "[dbus] Failed to install the horst sdbus object:  "
		          << strerror(-r) << std::endl;
		goto clean_return;
	}

	// register our service name
	r = sd_bus_request_name(this->bus, "moveii.horst", 0);
	if (r < 0) {
		std::cout << "[dbus] failed to acquire service name: "
		          << strerror(-r) << std::endl;
		goto clean_return;
	}

	// register the filedescriptor from
	// sd_bus_get_fd(bus) to libuv
	uv_poll_init(this->loop, &this->connection,
	             sd_bus_get_fd(this->bus));


	// make `this` reachable in callbacks.
	this->connection.data = this;

	// init the dbus-event-timer
	uv_timer_init(this->loop, &this->timer);
	this->timer.data = this;

	// register which signals are interesting for horst
	this->watch_for_signals();

	// process initial events and set up the
	// events and timers for subsequent calls
	on_dbus_ready(&this->connection, 0, 0);

	std::cout << "[dbus] listner initialized " << std::endl;
	return 0;

clean_return:
	sd_bus_slot_unref(this->bus_slot);
	sd_bus_unref(this->bus);
	this->bus = nullptr;

	return 1;
}


int DBusConnection::close() {
	// TODO: maybe this memoryerrors when the loop actually
	//       does the cleanup. we have to wait for the callback.
	uv_close((uv_handle_t *) &this->timer, nullptr);

	uv_poll_stop(&this->connection);

	sd_bus_close(this->bus);
	sd_bus_slot_unref(this->bus_slot);
	sd_bus_unref(this->bus);
	return 0;
}


Satellite *DBusConnection::get_sat() {
	return this->satellite;
}


sd_bus *DBusConnection::get_bus() {
	return this->bus;
}


void DBusConnection::emit_action_done(bool success, id_t action) {
	int r = sd_bus_emit_signal(
		this->bus,
		"/moveii/horst",
		"moveii.horst",
		"actionDone",
		"bt",
		success,
		action
	);
	if (r < 0) {
		std::cout << "failed to emit action_done" << std::endl;
	}

	// the signal can be sent out
	this->update_events();
}


void DBusConnection::watch_for_signals() {

	// set up watched signals
	// https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-routing-match-rules

	int r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"sender='moveii.pl',"
		"member='payloadMeasurementDone'",
		[] (sd_bus_message * /*m*/,
		    void * /*userdata*/,
		    sd_bus_error * /*ret_error*/) -> int {

			// DBusConnection *this_ = (DBusConnection *) userdata;

			std::cout << "[dbus] payload measurement done." << std::endl;

			// TODO: mark the done in the state table

			return 0;
		},
		this
	);
	if (r < 0) {
		std::cout << "Failed to add payload done match" << std::endl;
	}

	/* Ugly hack to get this Satellite into the callback :/ */
	globalsat = this->get_sat();

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"sender='moveii.thm',"
		"member='thmStateChange'",
		[] (sd_bus_message* m, void*, sd_bus_error*) -> int {
			uint8_t thmlevel;
			int r;

			r = sd_bus_message_read(m, "y", &thmlevel);
			if (r < 0) {
				std::cout << "[dbus] Failed to receive THM state change!" << std::endl;
				return 0;
			}
			std::cout << "[dbus] THM state changed to " << (int) thmlevel << std::endl;

			/* Generate fact and send it to state logic */
			auto req = std::make_shared<THMSignal>(static_cast<THM::overall_temp>(thmlevel));
			globalsat->on_event(std::move(req));

			return 0;
		},
		this
	);
	if (r < 0) {
		std::cout << "Failed to add thm temperature level x match" << std::endl;
	}

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"sender='moveii.eps',"
		"member='epsChargeStateChange'",
		[] (sd_bus_message * /*m*/,
		    void * /*userdata*/,
		    sd_bus_error * /*ret_error*/) -> int {

			// DBusConnection *this_ = (DBusConnection *) userdata;

			std::cout << "[dbus] eps battery level x." << std::endl;

			// TODO: mark the information in the state table

			return 0;
		},
		this
	);
	if (r < 0) {
		std::cout << "Failed to add eps battery level x match" << std::endl;
	}
}

}  // horst
