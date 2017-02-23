#include "dbus.h"

#include <iostream>

#include "../event/debugstuff.h"
#include "../event/eps_signal.h"
#include "../event/leop_signal.h"
#include "../event/adcs_signal.h"
#include "../event/manualmode_req.h"
#include "../event/req_procedure_call.h"
#include "../event/req_shell_command.h"
#include "../event/safemode_req.h"
#include "../event/thm_signal.h"
#include "../logger.h"
#include "../satellite.h"
#include "../state/thm.h"
#include "../util.h"


namespace horst {

/*
 * callback method used by libuv to notify when
 * dbus messages can be processed
 */
static void on_dbus_ready(uv_poll_t *handle, int /*status*/, int events) {
	LOG_DEBUG("[dbus] Bus is ready for some events: " + std::to_string(events));

	DBusConnection *connection = (DBusConnection *)handle->data;

	sd_bus *bus = connection->get_bus();

	// let dbus handle the requests available
	while (true) {
		int r = sd_bus_process(bus, nullptr);
		if (r < 0) {
			LOG_WARN("[dbus] Failed to process bus: : " + std::string(strerror(-r)));
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
	bus_slot{nullptr} { }


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
		LOG_ERROR(3, "[dbus] exec() failed to parse parameters: " + std::string(strerror(-r)));
		return r;
	}

	LOG_INFO("[dbus] Exec command: " + std::string(command));
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
		LOG_ERROR(3, "[dbus] run() failed to parse parameters: " + std::string(strerror(-r)));
		return r;
	}

	LOG_INFO("[dbus] Run procedure: " + std::string(name));
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
		LOG_ERROR(3, "[dbus] set() failed to parse parameters: " + std::string(strerror(-r)));
		return r;
	}

	LOG_INFO("[dbus] Request to set " + std::string(name));
	auto req = std::make_shared<DebugStuff>(name);
	this_->get_sat()->on_event(std::move(req));

	return sd_bus_reply_method_return(m, "x", 0);
}

static int dbus_safemode(sd_bus_message *m, void *userdata, sd_bus_error*) {
	DBusConnection *this_ = (DBusConnection *)userdata;
	bool safemode;
	int r = sd_bus_message_read(m, "y", &safemode);
	if (r < 0) {
		LOG_ERROR(3, "[dbus] safemode() failed to parse parameters: " + std::string(strerror(-r)));
		return r;
	}

	LOG_INFO("[dbus] Request to set safemode to " + std::to_string(safemode));
	auto req = std::make_shared<SafeModeReq>(safemode);
	this_->get_sat()->on_event(std::move(req));

	return sd_bus_reply_method_return(m, "b", true);
}

static int getBeaconData(sd_bus_message *m, void *userdata, sd_bus_error*) {
	DBusConnection *this_ = (DBusConnection *)userdata;
	State* state = this_->get_sat()->get_state();
	std::vector<uint8_t> data;
	sd_bus_message *retm;

	// Create new return message
	int r = sd_bus_message_new_method_return(m, &retm);
	if (r < 0) {
		std::cout << "Failed to create return message: " << strerror(-r) << std::endl;
		return 0;
	}

	// Fill data
	data.push_back((uint8_t) state->safemode);
	data.push_back((uint8_t) state->manualmode);
	data.push_back((uint8_t) (state->eps.battery_level >> 8));
	data.push_back((uint8_t) (state->eps.battery_level & 0xFF));
	data.push_back((uint8_t) state->thm.all_temp);
	data.push_back((uint8_t) 0); // TODO: Insert ADCS pointing
	data.push_back((uint8_t) 0); // TODO: Insert ADCS pointing requested
	data.push_back((uint8_t) 0); // TODO: Insert Payload
	data.push_back((uint8_t) 0); // TODO: Insert LEOP

	// Append data to message
	r = sd_bus_message_append_array(retm, 'y', &data[0], data.size());
	if (r < 0) {
		std::cout << "Failed to append data to message: " << strerror(-r) << std::endl;
		return 0;
	}

	// Send message on bus
	r = sd_bus_send(sd_bus_message_get_bus(m), retm, NULL);
	if (r < 0) {
		std::cout << "Failed to reply return message: " << strerror(-r) << std::endl;
		return 0;
	}

	sd_bus_message_unref(retm);
	return 0;
}

static int dbus_manualmode(sd_bus_message *m, void *userdata, sd_bus_error*) {
	DBusConnection *this_ = (DBusConnection *)userdata;
	bool manualmode;
	int r = sd_bus_message_read(m, "y", &manualmode);
	if (r < 0) {
		LOG_ERROR(3, "[dbus] manualmode() failed to parse parameters: " + std::string(strerror(-r)));
		return r;
	}

	LOG_INFO("[dbus] Request to set manualmode to " + std::to_string(manualmode));
	auto req = std::make_shared<ManualModeReq>(manualmode);
	this_->get_sat()->on_event(std::move(req));

	return sd_bus_reply_method_return(m, "b", true);
}

static const sd_bus_vtable horst_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("run", "s", "x", dbus_run, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("exec", "s", "x", dbus_exec, SD_BUS_VTABLE_UNPRIVILEGED),
	// debugging function: remove it!
	SD_BUS_METHOD("set", "s", "x", dbus_set, SD_BUS_VTABLE_UNPRIVILEGED),
	// b as input does not work. Reading it from the message seems to
	// destroy the userdata pointer (systemd bug?). Using y instead...
	SD_BUS_METHOD("safemode", "y", "b", dbus_safemode, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("manualmode", "y", "b", dbus_manualmode, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_SIGNAL("actionDone", "bt", 0),
	SD_BUS_METHOD("getBeaconData", "", "ay", getBeaconData, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_VTABLE_END
};


int DBusConnection::connect() {
	int r = sd_bus_open_system(&this->bus);

	if (r < 0) {
		LOG_ERROR(4, "[dbus] Failed to connect to bus: " + std::string(strerror(-r)));
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
		LOG_ERROR(10, "[dbus] Failed to install the horst sdbus object: " + std::string(strerror(-r)));
		goto clean_return;
	}

	// register our service name
	r = sd_bus_request_name(this->bus, "moveii.horst", 0);
	if (r < 0) {
		LOG_ERROR(5, "[dbus] Failed to acquire service name: " + std::string(strerror(-r)));
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

	LOG_DEBUG("[dbus] Listener initialized");
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
		LOG_WARN("[dbus] Failed to emit action_done: : " + std::string(strerror(-r)));
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

			LOG_INFO("[dbus] Payload measurement done");

			// TODO: mark the done in the state table

			return 0;
		},
		this
	);
	if (r < 0) {
		LOG_ERROR(6, "[dbus] Failed to add payload done match: " + std::string(strerror(-r)));
	}

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"sender='moveii.thm',"
		"member='thmStateChange'",
		[] (sd_bus_message* m, void *userdata, sd_bus_error*) -> int {
			DBusConnection *this_ = (DBusConnection *) userdata;
			uint8_t thmlevel;
			int r;

			r = sd_bus_message_read(m, "y", &thmlevel);
			if (r < 0) {
				LOG_WARN("[dbus] Failed to receive THM state change: " + std::string(strerror(-r)));
				return 0;
			}
			LOG_INFO("[dbus] THM state changed to: " + std::to_string(thmlevel));

			/* Generate fact and send it to state logic */
			auto req = std::make_shared<THMSignal>(static_cast<THM::overall_temp>(thmlevel));
			this_->get_sat()->on_event(std::move(req));

			return 0;
		},
		this
	);
	if (r < 0) {
		LOG_ERROR(6, "[dbus] Failed to add temperature level x match: " + std::string(strerror(-r)));
	}

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"sender='moveii.eps',"
		"member='epsChargeStateChange'",
		[] (sd_bus_message* m, void *userdata, sd_bus_error*) -> int {
			DBusConnection *this_ = (DBusConnection *) userdata;
			uint16_t bat;
			int r;

			r = sd_bus_message_read(m, "q", &bat);
			if (r < 0) {
				LOG_WARN("[dbus] Failed to receive EPS battery state change: " + std::string(strerror(-r)));
				return 0;
			}
			LOG_INFO("[dbus] EPS battery state changed to: " + std::to_string(bat));

			/* Generate fact and send it to state logic */
			auto req = std::make_shared<EPSSignal>(bat);
			this_->get_sat()->on_event(std::move(req));

			return 0;
		},
		this
	);
	if (r < 0) {
		LOG_ERROR(6, "[dbus] Failed to add EPS battery level x match: " + std::string(strerror(-r)));
	}

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"sender='moveii.leop',"
		"member='leopStateChange'",
		[] (sd_bus_message* m, void *userdata, sd_bus_error*) -> int {
			DBusConnection *this_ = (DBusConnection *) userdata;
			uint8_t leop;
			int r;

			r = sd_bus_message_read(m, "y", &leop);
			if (r < 0) {
				std::cout << "[dbus] Failed to receive LEOP state change!" << std::endl;
				return 0;
			}
			std::cout << "[dbus] LEOP state changed to " << (int) leop << std::endl;

			/* Generate fact and send it to state logic */
			auto sig = std::make_shared<LEOPSignal>(static_cast<State::leop_seq>(leop));
			this_->get_sat()->on_event(std::move(sig));

			return 0;
		},
		this
	);
	if (r < 0) {
		std::cout << "Failed to add LEOP state change match" << std::endl;
	}
	
	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"sender='moveii.adcs',"
		"member='adcsStateChange'",
		[] (sd_bus_message* m, void *userdata, sd_bus_error*) -> int {
			DBusConnection *this_ = (DBusConnection *) userdata;
			ADCS::adcs_state adcs_status;
			int r;

			r = sd_bus_message_read(m, "y", &adcs_status);
			if (r < 0) {
				std::cout << "[dbus] Failed to receive ADCS state change!" << std::endl;
				return 0;
			}
			std::cout << "[dbus] ADCS state changed to " << (int) adcs_status << std::endl;

			/* Generate fact and send it to state logic */
			auto sig = std::make_shared<ADCSSignal>(static_cast<ADCS::adcs_state>(adcs_status));
			this_->get_sat()->on_event(std::move(sig));

			return 0;
		},
		this
	);
	if (r < 0) {
	        LOG_ERROR(6, "[dbus] Failed to add ADCS state change match: " + std::string(strerror(-r)));
	}
}

}  // horst
