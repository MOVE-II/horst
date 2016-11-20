#include "client.h"

#include <cstdio>
#include <iostream>
#include <unistd.h>

#include "satellite.h"


namespace horst {

Client::Client(Satellite *satellite)
	:
	close_stream{true},
	connection{std::make_unique<uv_tcp_t>()},
	server{nullptr} {

	// initialize the stream client handle:
	uv_tcp_init(satellite->get_loop(), this->connection.get());
	this->connection->data = this;
}


Client::Client(Client &&other)
	:
	connection{std::move(other.connection)},
	server{other.server} {

	other.close_stream = false;
}


Client &Client::operator =(Client &&other) {
	this->connection = std::move(other.connection);
	this->server = other.server;
	other.close_stream = false;

	return *this;
}


Client::~Client() {
	// this is false when we moved the data to another client
	// so this one just decays and the connection stays open.

	if (this->close_stream) {
		this->close();
	}
}


bool Client::accept(uv_stream_t *server) {
	bool success = uv_accept(server, this->get_stream()) == 0;

	if (success) {
		this->register_callbacks();
		this->server = server;
	}

	return success;
}


void Client::register_callbacks() {
	// use the first lambda to allocate memory
	// and the second lambda to deallocate it and
	// call the callback.

	std::cout << "register client callbacks" << std::endl;

	uv_read_start(
		this->get_stream(),

		// memory allocation:
		[] (uv_handle_t * /*handle*/,
		    size_t suggested_size,
		    uv_buf_t *buf) {

			// allocate the memory directly!
			// deallocation happens in the lambda below.
			std::cout << "alloc(" << suggested_size << ")" << std::endl;
			char *mem = new char[suggested_size];
			*buf = uv_buf_init(mem, suggested_size);
			printf("%p\n", mem);
		},

		// called when data was read:
		[] (uv_stream_t */*stream*/,
		    ssize_t nread,
		    const uv_buf_t *buf) {

			// wrap the memory in a unique_ptr for char,
			// but delete a char[]
			// this gets deallocated when it goes out of scope here
			// => memory is kept valid for the callback
			//    and its exceptions
			std::unique_ptr<char[]> holder(buf->base);

			std::cout << "n=" << nread << std::endl;
			write(0, buf->base, nread);
			std::cout << "==" << std::endl;

			if (nread < 0) {
				// TODO: callback with nullptr
				std::cout << "connection lost" << std::endl;
			}

			//Client *this_ = (Client *)stream->data;
			// TODO: call this_->callback(buf->base, nread)

			printf("free %p\n", buf->base);
		}
	);
}


void Client::close() {
	uv_close(this->get_handle(), nullptr);
}


uv_stream_t *Client::get_stream() {
	return reinterpret_cast<uv_stream_t *>(this->connection.get());
}

uv_handle_t *Client::get_handle() {
	return reinterpret_cast<uv_handle_t *>(this->connection.get());
}


} // horst
