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

	// register callbacks for data receiving
	uv_read_start(
		this->get_stream(),

		// memory allocation:
		[] (uv_handle_t * /*handle*/,
		    size_t suggested_size,
		    uv_buf_t *buf) {

			// allocate the memory directly!
			// deallocation happens in the lambda below.
			char *mem = new char[suggested_size];
			*buf = uv_buf_init(mem, suggested_size);
		},

		// called when data was read:
		[] (uv_stream_t *stream,
		    ssize_t nread,
		    const uv_buf_t *buf) {

			// wrap the memory in a unique_ptr for char,
			// but delete a char[]
			// this gets deallocated when it goes out of scope here
			// => memory is kept valid for the callback
			//    and its exceptions
			std::unique_ptr<char[]> holder(buf->base);

			Client *this_ = (Client *)stream->data;
			char *data = nullptr;

			if (nread < 0) {
				// no longer use the callback
				uv_read_stop(stream);

				// handle the connection loss
				this_->connection_lost(nread);
				return;
			}
			else if (nread > 0) {
				data = buf->base;
			}

			// provide the data
			this_->data_received(data, nread);
		}
	);
}


void Client::data_received(const char *data, int size) {
	if (size > 0) {
		write(0, data, size);
	}
}

void Client::connection_lost(int code) {
	if (code == UV_EOF) {
		std::cout << "connection closed" << std::endl;
	} else {
		std::cout << "connection lost: " << code << std::endl;
	}
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
