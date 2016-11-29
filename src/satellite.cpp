#include "satellite.h"

#include <iostream>
#include <memory>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <systemd/sd-bus.h>
#include <unistd.h>

#include "log.h"
#include "util.h"


namespace horst {

Satellite::Satellite(const arguments &args)
	:
	args{args} {

	uv_loop_init(&this->loop);
}

Satellite::~Satellite() {
	uv_loop_close(&this->loop);
}


int Satellite::run() {
	log("starting up connections...");

	if (this->listen_tcp(this->args.port, &this->server)) {
		log("failed to set up tcp socket.");
		return 1;
	}

	if (this->listen_dbus()) {
		log("failed to listen on dbus.");
		return 1;
	}

	return uv_run(&this->loop, UV_RUN_DEFAULT);
}


int Satellite::listen_tcp(int port, uv_tcp_t *server) {
	int ret;

	uv_tcp_init(&this->loop, server);

	// listen on tcp socket.
	sockaddr_in6 listen_addr;
	ret = uv_ip6_addr("::", port, &listen_addr);

	if (ret) {
		std::cerr << "can't create liste addr: "
		          << uv_strerror(ret) << std::endl;
		return 1;
	}

	ret = uv_tcp_bind(server, (const sockaddr*) &listen_addr, 0);

	if (ret) {
		std::cerr << "can't bind to socket: "
		          << uv_strerror(ret) << std::endl;
		return 1;
	}

	// make `this` reachable in callbacks.
	server->data = this;

	// start to listen on the tcp socket
	ret = uv_listen(
		(uv_stream_t *)server,
		4,  // < kernel connection queue size

		// when a new connection was received:
		[] (uv_stream_t *server, int status) {

			if (status < 0) {
				std::cerr << "New connection error: "
				          << uv_strerror(status) << std::endl;
				return;
			}

			std::cout << "new connection received" << std::endl;

			Satellite *this_ = (Satellite *) server->data;

			Client client{this_};

			std::cout << "client created" << std::endl;

			// accept the connection on the listening socket
			if (client.accept(server)) {
				std::cout << "accepted client." << std::endl;
				this_->add_client(std::move(client));
			}
			else {
				std::cout << "failed to accept error" << std::endl;
			}
		}
	);

	if (ret) {
		std::cerr << "Listen error: "
		          << uv_strerror(ret) << std::endl;
		return 1;
	}

	return 0;
}

static const sd_bus_vtable horst_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("start", "xx", "x", nullptr, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_VTABLE_END
};


int Satellite::listen_dbus() {
	// dbus setup:
	// TODO use libuv for waiting for sdbus socket.
	// https://github.com/systemd/systemd/blob/493fd52f1ada36bfe63301d4bb50f7fd2b38c670/src/libsystemd/sd-bus/sd-bus.c#L2904

	sd_bus_slot *slot = nullptr;
	sd_bus *bus = nullptr;
	int r;

	// open system bus
	r = sd_bus_open_system(&bus);
	if (r < 0) {
		fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
		goto finish;
	}

	// install the object
	r = sd_bus_add_object_vtable(
		bus,
		&slot,
		"/warr/moveII/horst",  // object path
		"warr.moveII.horst",   // interface name
		horst_vtable,
		nullptr
	);

	if (r < 0) {
		std::cerr << "Failed to issue method call: " << strerror(-r) << std::endl;
		goto finish;
	}

	/* Take a well-known service name so that clients can find us */
	r = sd_bus_request_name(bus, "warr.moveII.horst", 0);
	if (r < 0) {
		std::cerr << "Failed to acquire service name: " << strerror(-r) << std::endl;
		goto finish;
	}

	// TODO: register the filedescriptor from
	//       sd_bus_get_fd(bus) to libuv

finish:
	sd_bus_slot_unref(slot);
	sd_bus_unref(bus);

	return 0;
}


uv_loop_t *Satellite::get_loop() {
	return &this->loop;
}


void Satellite::add_client(Client &&client) {
	this->clients.push_back(std::move(client));
}

void Satellite::enqueue(ControlMessage &&msg) {
	this->commands.push(std::move(msg));
}


} // horst
