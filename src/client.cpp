#include "client.h"

#include <iostream>

#include "satellite.h"


namespace horst {

Client::Client(Satellite *satellite, uv_stream_t *server)
	:
	close_stream{true},
	server{server} {

	uv_tcp_init(satellite->get_loop(), &this->connection);
	this->connection.data = this;
}


Client::Client(Client &&other)
	:
	connection{other.connection},
	server{other.server} {

	other.close_stream = false;
}


Client &Client::operator =(Client &&other) {
	this->connection = other.connection;
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

void Client::register_callbacks() {
	// use the first lambda to allocate memory
	// and the second lambda to deallocate it and
	// call the callback.
	uv_read_start(
		this->get_stream(),

		// memory allocation:
		[] (uv_handle_t *handle,
		    size_t suggested_size,
		    uv_buf_t *buf) {

			// allocate the memory directly!
			// deallocation happens in the lambda below.
			char *mem = new char [suggested_size];
			*buf = uv_buf_init(mem, suggested_size);
		},

		// called when data was read:
		[] (uv_stream_t *stream,
		    ssize_t nread,
		    const uv_buf_t *buf) {

			Client *this_ = (Client *)stream->data;

			// wrap the memory in a unique_ptr for char,
			// but delete a char[]
			// this gets deallocated when it goes out of scope here
			// => memory is kept valid for the callback
			//    and its exceptions
			std::unique_ptr<char[]> holder(
				buf->base
			);

			std::cout << "n=" << nread
			          << " -> " << buf->base << std::endl
			;

			// TODO: call this_->callback(buf->base, nread)
		}
	);
}

void Client::close() {
	uv_close(this->get_handle(), nullptr);
}


uv_stream_t *Client::get_stream() {
	return (uv_stream_t*) &this->connection;
}

uv_handle_t *Client::get_handle() {
	return (uv_handle_t*) &this->connection;
}


} // horst
