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

	uv_loop_close(&this->loop);
	sd_bus_close(this->bus);
}


int Satellite::run() {
	log("[satellite] starting up connections...");

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
	return uv_run(&this->loop, UV_RUN_DEFAULT);
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
				this_->add_client(std::move(client));
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


/* stupid dbus test function to provide multiplication */
int method_multiply(sd_bus_message *m,
                           void * /*userdata*/,
                           sd_bus_error * /*ret_error*/) {
	int64_t x, y;
	int r;

	// xx = two 64 bit numbers
	r = sd_bus_message_read(m, "xx", &x, &y);
	if (r < 0) {
		std::cout << "Failed to parse parameters: "
		          << strerror(-r) << std::endl;
		return r;
	}

	return sd_bus_reply_method_return(m, "x", x * y);
}


static const sd_bus_vtable horst_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("multiply", "xx", "x", method_multiply, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_VTABLE_END
};


int Satellite::listen_dbus() {
	// dbus setup:
	// TODO use libuv for waiting for sdbus socket.
	// https://github.com/systemd/systemd/blob/493fd52f1ada36bfe63301d4bb50f7fd2b38c670/src/libsystemd/sd-bus/sd-bus.c#L2904

	sd_bus_slot *slot = nullptr;
	int r;

	// open bus: system or user
	r = sd_bus_open_user(&this->bus);
	if (r < 0) {
		std::cout << "failed to connect to bus: "
		          << strerror(-r) << std::endl;
		goto finish;
	}

	// install the object
	r = sd_bus_add_object_vtable(
		this->bus,
		&slot,
		"/warr/moveII/horst",  // object path
		"warr.moveII.horst",   // interface name
		horst_vtable,
		nullptr
	);

	if (r < 0) {
		std::cout << "[dbus] failed to issue method call: "
		          << strerror(-r) << std::endl;
		goto finish;
	}

	/* Take a well-known service name so that clients can find us */
	r = sd_bus_request_name(this->bus, "warr.moveII.horst", 0);
	if (r < 0) {
		std::cout << "[dbus] failed to acquire service name: "
		          << strerror(-r) << std::endl;
		goto finish;
	}

	// TODO: register the filedescriptor from
	//       sd_bus_get_fd(bus) to libuv


	/*
	// TODO: use the libuv loop instead of this one.
	while (true) {
		r = sd_bus_process(bus, nullptr);
		if (r < 0) {
			std::cout << "[dbus] failed to process bus: "
			          << strerror(-r) << std::endl;
			goto finish;
		}
		if (r > 0)
			continue;

		r = sd_bus_wait(bus, (uint64_t) -1);
		if (r < 0) {
			std::cout << "[dbus] failed to wait on bus: "
			          << strerror(-r) << std::endl;
			goto finish;
		}
	}
	*/

	return 0;

finish:
	sd_bus_slot_unref(slot);
	sd_bus_unref(this->bus);

	this->bus = nullptr;

	return 1;
}


uv_loop_t *Satellite::get_loop() {
	return &this->loop;
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


void Satellite::remove_action(id_t id) {
	auto pos = this->actions.find(id);
	if (pos != std::end(this->actions)) {
		this->actions.erase(pos);
	} else {
		std::cout << "an unknown action just finished..." << std::endl;
	}
}


void Satellite::on_event(std::unique_ptr<Event> &&event) {
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
		std::cout << "performing: " << action_m->describe() << std::endl;

		// store the action.
		id_t id = this->add_action(std::move(action_m));

		Action *action = this->get_action(id);
		action->call_when_done([this, id] (Action *) {
			this->remove_action(id);
		});

		// perform the action, this may just enqueue it in the event loop.
		action->perform(this);
	}
}


} // horst
