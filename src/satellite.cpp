#include "satellite.h"

#include <iostream>
#include <memory>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "action/action.h"
#include "client/tcp_client.h"
#include "log.h"
#include "util.h"

namespace horst {

Satellite::Satellite(const arguments &args)
	:
	args{args},
	next_id{0},
	bus{nullptr} {

	uv_loop_init(&this->loop);
}


Satellite::~Satellite() {
	std::cout << "[satellite] destroying..." << std::endl;

	sd_bus_close(this->bus);
	sd_bus_slot_unref(this->bus_slot);
	sd_bus_unref(this->bus);
	uv_poll_stop(&this->dbus_connection);
	uv_loop_close(&this->loop);
}


int Satellite::run() {
	log("[satellite] starting up connections...");
	int ret;

	if (this->listen_tcp(this->args.port)) {
		log("[satellite] failed to set up tcp socket.");
		return 1;
	}

	if (this->listen_dbus()) {
		log("[satellite] failed to listen on dbus.");
		return 1;
	}

	if (this->listen_s3tp(this->args.port)) {
		log("[satellite] failed to listen on s3tp.");
		return 1;
	}

	// let the event loop run forever.
	log("[satellite] Starting event loop");
	ret = uv_run(&this->loop, UV_RUN_DEFAULT);
	log("[satellite] Stopping event loop");
	return ret;
}


int Satellite::listen_tcp(int port) {
	int ret;

	uv_tcp_init(&this->loop, &this->server);

	std::cout << "[satellite] listening on port "
	          << port << "..." << std::endl;

	// listen on tcp socket.
	sockaddr_in6 listen_addr;
	ret = uv_ip6_addr("::", port, &listen_addr);

	if (ret) {
		std::cout << "[satellite] can't create liste addr: "
		          << uv_strerror(ret) << std::endl;
		return 1;
	}

	ret = uv_tcp_bind(&this->server, (const sockaddr*) &listen_addr, 0);

	if (ret) {
		std::cout << "[satellite] can't bind to socket: "
		          << uv_strerror(ret) << std::endl;
		return 1;
	}

	// make `this` reachable in callbacks.
	this->server.data = this;

	// start to listen on the tcp socket
	ret = uv_listen(
		(uv_stream_t *)&this->server,
		4,  // < kernel connection queue size

		// when a new connection was received:
		[] (uv_stream_t *server, int status) {

			if (status < 0) {
				std::cout << "[satellite] new connection error: "
				          << uv_strerror(status) << std::endl;
				return;
			}

			std::cout << "[satellite] new connection received" << std::endl;

			Satellite *this_ = (Satellite *) server->data;
			auto client = std::make_unique<TCPClient>(this_);

			// accept the connection on the listening socket
			if (client->accept(server)) {
				id_t id = this_->add_client(std::move(client));
				Client *stored_client = this_->get_client(id);

				// when the client connection closes,
				// remove the handle to the stored client.
				stored_client->call_on_close(
					[this_, id] (Client *) {
						this_->remove_client(id);
					}
				);
			}
			else {
				std::cout << "[satellite] failed to accept tcp client"
				          << std::endl;
			}
		}
	);

	if (ret) {
		std::cout << "[satellite] listen error: "
		          << uv_strerror(ret) << std::endl;
		return 1;
	}

	return 0;
}


int Satellite::listen_s3tp(int /*port*/) {
	// TODO register s3tp_connection to event loop.
	return 0;
}


void Satellite::on_dbus_ready(uv_poll_t *handle,
                              int /*status*/,
                              int /*events*/) {
	int r;
	uint64_t usec;

	sd_bus *this_bus = ((Satellite *)handle->data)->get_bus();

	log("->Enter dbus events call back ");

	int current_events = sd_bus_get_events(this_bus);
	sd_bus_get_timeout(this_bus, &usec);
	uv_poll_start(handle, current_events, &on_dbus_ready);

	// let dbus handle the requests
	r = sd_bus_process(this_bus, nullptr);

	if (r < 0) {
		std::cout << "Failed to process bus:  "
		          << strerror(-r) << std::endl;
		return;
	}
	else if (r > 0) {
		/// we processed a request, try to process another one, right-away
		log("-----a dbus request has been processed ");
	}

	log("<-Exit dbus events call back ");
}

static int start(sd_bus_message *m,
                 void * /*userdata*/,
                 sd_bus_error * /*ret_error*/) {

	log("***method START in horst service is being called ! ");
	return sd_bus_reply_method_return(m, "x", 0);
}

static int stop_callback(sd_bus_message *m,
                         void * /*userdata*/,
                         sd_bus_error * /*ret_error*/) {

	int r;
	const char *path;

	log("***method stop_callback in horst service is being called ! ");

	r = sd_bus_message_read(m, "s", &path);
	if (r < 0) {
		std::cout << "Failed to parse response message: "
		          << strerror(-r) << std::endl;
		goto exit;
	}

	printf("Machine ID is %s.\n", path);
exit:
	return sd_bus_reply_method_return(m, "x", 0);
}


static int stop(sd_bus_message *m,
                void *userdata,
                sd_bus_error * /*ret_error*/) {
	int r = 0;
	int input;

	log("***method STOP in horst service is being called ! ");

	/* Read the parameters */
	r = sd_bus_message_read(m, "x", &input);
	if (r < 0) {
		std::cout << "Failed to parse parameters:  " << strerror(-r)
		          << std::endl;
		return r;
	}

	if (input == 0) {
		/* example from linux  bus */
		r = sd_bus_call_method_async(
			((Satellite *)userdata)->get_bus(),
			((Satellite *)userdata)->get_bus_slot(),
			"org.gnome.gedit",                    /* service to contact */
			"/com/canonical/unity/gtk/window/42", /* object path */
			"org.freedesktop.DBus.Peer",          /* interface name */
			"GetMachineId",                       /* method name */
			stop_callback,
			userdata,
			""
		);
	} else {
		/* example from move2  bus */
		r = sd_bus_call_method_async(
			((Satellite *)userdata)->get_bus(),
			((Satellite *)userdata)->get_bus_slot(),
			"warr.moveII.horst",  /* service to contact */
			"/warr/moveII/horst", /* object path */
			"warr.moveII.horst",  /* interface name */
			"stop",               /* method name */
			stop_callback,
			userdata,
			"x",
			input
		);
	}

	if (r < 0) {
		std::cout << "Failed to issue method call: " << strerror(-r)
		          << std::endl;
		return r;
	}

	return sd_bus_reply_method_return(m, "x", 0);
}

static const sd_bus_vtable horst_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("start", "", "x", start, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("stop", "x", "x", stop, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_VTABLE_END
};

int Satellite::listen_dbus() {
	int cuurent_events;
	uint64_t usec;
	int r;

	// open bus: system or user
	this->bus_slot = nullptr;
	this->bus = nullptr;
	r = sd_bus_open_user(&this->bus);

	if (r < 0) {
		std::cout << "[dbus] Failed to connect to system/user bus:  "
		          << strerror(-r) << std::endl;
		goto clean_return;
	}

	// install the horst object
	r = sd_bus_add_object_vtable(
		this->bus,
		&this->bus_slot,
		"/warr/moveII/horst",  // object path
		"warr.moveII.horst",   // interface name
		horst_vtable,
		this
	);

	if (r < 0) {
		std::cout << "[dbus] Failed to install the horst sdbus object:  "
		          << strerror(-r) << std::endl;
		goto clean_return;
	}

	/* Take a well-known service name so that clients can find us */
	r = sd_bus_request_name(this->bus, "warr.moveII.horst", 0);
	if (r < 0) {
		std::cout << "[dbus] failed to acquire service name: "
		          << strerror(-r) << std::endl;
		goto clean_return;
	}


	// ### two events are already queued, we need to process them
	//     in order to allow libuv to start the poll on the fd
	r = sd_bus_process(this->bus, nullptr);
	r = sd_bus_process(this->bus, nullptr);


	// register the filedescriptor from
	//       sd_bus_get_fd(bus) to libuv
	cuurent_events = sd_bus_get_events(this->bus);
	sd_bus_get_timeout(this->bus, &usec);
	uv_poll_init(&this->loop, &this->dbus_connection,
	             sd_bus_get_fd(this->bus));


	// make `this` reachable in callbacks.
	this->dbus_connection.data = this;
	uv_poll_start(&this->dbus_connection,
	              UV_READABLE | UV_WRITABLE | UV_DISCONNECT,
	              &Satellite::on_dbus_ready);

	log("[dbus] listner initialized ");
	return 0;

clean_return:
	sd_bus_slot_unref(this->bus_slot);
	sd_bus_unref(this->bus);
	this->bus = nullptr;

	return 1;
}


uv_loop_t *Satellite::get_loop() {
	return &this->loop;
}


sd_bus *Satellite::get_bus() {
	return this->bus;
}

sd_bus_slot **Satellite::get_bus_slot() {
	return &this->bus_slot;
}


id_t Satellite::add_client(std::unique_ptr<Client> &&client) {
	this->clients.emplace(this->next_id, std::move(client));
	return this->next_id++;
}


id_t Satellite::add_action(std::unique_ptr<Action> &&action) {
	this->actions.emplace(this->next_id, std::move(action));
	return this->next_id++;
}


Client *Satellite::get_client(id_t id) {
	auto loc = this->clients.find(id);
	if (loc == std::end(this->clients)) {
		return nullptr;
	} else {
		return loc->second.get();
	}
}


Action *Satellite::get_action(id_t id) {
	auto loc = this->actions.find(id);
	if (loc == std::end(this->actions)) {
		return nullptr;
	} else {
		return loc->second.get();
	}
}

const Procedure *Satellite::get_procedure(const std::string &name) const {
	return this->procedures.get_procedure(name);
}


void Satellite::remove_action(id_t id) {
	auto pos = this->actions.find(id);
	if (pos != std::end(this->actions)) {
		this->actions.erase(pos);
	} else {
		std::cout << "attempt to remove an unknown action..."
		          << std::endl;
	}
}


void Satellite::remove_client(id_t id) {
	auto pos = this->clients.find(id);
	if (pos != std::end(this->clients)) {
		this->clients.erase(pos);
	} else {
		std::cout << "attempt to remove an unknown client..."
		          << std::endl;
	}
}


void Satellite::on_event(std::shared_ptr<Event> &&event) {
	// called for each event the satellite receives
	// it may come from earth or any other subsystem

	// if the event is a fact, update the current state
	if (event->is_fact()) {
		event->update(this->current_state);
	}

	// create the target state as a copy of the current state
	State target_state = this->current_state.copy();

	// it the event is a request (i.e. not a fact),
	// update it in the target state
	if (not event->is_fact()) {
		event->update(target_state);
	}

	// determine the actions needed to reach the target state
	auto actions = this->current_state.transform_to(target_state);

	for (auto &action_m : actions) {
		// store the action
		id_t id = this->add_action(std::move(action_m));

		// and fetch its new location
		Action *action = this->get_action(id);

		std::cout << "[action] run #" << id << ": "
		          << action->describe() << std::endl;

		// perform the action, this may just enqueue it in the event loop.
		// the callback is executed when the action is done.
		action->perform(
			this,
			[this, id] (bool success, Action *) {
				if (not success) {
					std::cout << "[action] #"
					          << id << " failed!" << std::endl;
				}
				else {
					std::cout << "[action] #"
					          << id << " succeeded!" << std::endl;
				}
				this->remove_action(id);
			}
		);
	}
}


} // horst
