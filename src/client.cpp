#include "client.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "ctlmessage.h"
#include "satellite.h"


namespace horst {

Client::Client(Satellite *satellite)
	:
	satellite{satellite},
	close_stream{true},
	connection{std::make_unique<uv_tcp_t>()},
	server{nullptr},
	buf{std::make_unique<char[]>(this->max_buf_size)},
	buf_used{0} {

	// initialize the stream client handle:
	uv_tcp_init(this->satellite->get_loop(), this->connection.get());
	this->connection->data = this;

	std::cout << "client constructor to " << (void *)this << std::endl;
}


Client::Client(Client &&other)
	:
	satellite{other.satellite},
	connection{std::move(other.connection)},
	server{other.server},
	buf{std::move(other.buf)},
	buf_used{other.buf_used} {

	std::cout << "client after move " << (void *)this << std::endl;

	// update thisptr after move!
	this->connection->data = this;

	other.close_stream = false;
}


Client &Client::operator =(Client &&other) {
	this->satellite = other.satellite;
	this->connection = std::move(other.connection);
	this->server = other.server;
	this->buf = std::move(other.buf);
	this->buf_used = other.buf_used;

	// update thisptr after move!
	this->connection->data = this;

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
				// handle the connection loss
				this_->connection_lost(nread);
				return;
			}
			else if (nread > 0) {
				data = holder.get();
			}

			// provide the data
			this_->data_received(data, nread);
		}
	);
}


void Client::data_received(const char *data, size_t size) {
	if (this->buf_used + size >= this->max_buf_size) {
		std::cout << "receive buffer too full" << std::endl;
		this->close();
		return;
	}

	// The following "few" lines implement in C++ what Python does:
	//   ... buf = bytearray() ...
	//   buf.extend(data)
	//   npos = buf.rfind(b'\n')
	//   for line in buf[:npos].split(b'\n'):
	//       # thing with line
	//   del buf[:npos]
	size_t ncpy = std::min(size, (this->max_buf_size - this->buf_used - 1));

	// copy data at the beginning
	std::memcpy(&this->buf[this->buf_used], data, ncpy);

	this->buf[this->buf_used + ncpy] = '\0';
	this->buf_used += ncpy;

	char split_at = '\n';

	std::string buf_str{this->buf.get()};
	size_t npos = buf_str.rfind(split_at);

	if (npos == std::string::npos) {
		// no newline found, just wait for more data.
		return;
	}

	std::stringstream lines{buf_str.substr(0, npos)};
	std::string part;
	std::vector<std::string> seglist;

	while (lines.good()) {
		std::getline(lines, part, split_at);
		seglist.push_back(part);
	}

	std::memmove(this->buf.get(), &this->buf[npos + 1],
	             this->buf_used - npos + 1);

	this->buf_used -= npos + 1;

	for (auto &entry : seglist) {
		std::cout << "got CMD: " << entry << std::endl;
		ControlMessage cmd{entry};

		this->satellite->enqueue(std::move(cmd));
	}
}


void Client::connection_lost(int code) {
	if (code == UV_EOF) {
		std::cout << "connection closed" << std::endl;
	} else {
		std::cout << "connection lost: " << code << std::endl;
	}

	// no longer use the callback
	uv_read_stop(this->get_stream());

	this->close();
	// TODO: remove from parent container if necessary
}


void Client::close() {
	if (not uv_is_closing(this->get_handle())) {
		uv_close(this->get_handle(), nullptr);
	}
}


uv_stream_t *Client::get_stream() {
	return reinterpret_cast<uv_stream_t *>(this->connection.get());
}

uv_handle_t *Client::get_handle() {
	return reinterpret_cast<uv_handle_t *>(this->connection.get());
}


} // horst
