#include "tcp_client.h"

#include <iostream>

#include "../logger.h"
#include "../satellite.h"


namespace horst {

TCPClient::TCPClient(Satellite *satellite,
                     close_cb_t on_close)
	:
	Client{satellite, on_close},
	server{nullptr} {

	// initialize the stream client handle:
	uv_tcp_init(this->satellite->get_loop(), &this->connection);
	this->connection.data = this;
}


TCPClient::~TCPClient() {}


bool TCPClient::accept(uv_stream_t *server) {
	bool success = uv_accept(server, this->get_stream()) == 0;

	if (success) {
		this->start_receiving();
		this->server = server;
	}

	return success;
}


void TCPClient::start_receiving() {
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
			std::unique_ptr<char[]> holder{buf->base};

			TCPClient *this_ = (TCPClient *)stream->data;
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


void TCPClient::send(const char *buf, size_t len) {

	// the request has to live until the callback was done.
	uv_write_t *req = new uv_write_t;

	// we may store `this` in the request to access it in the lambda.
	// req->data = this;

	// create a copy of the data to be send.
	char *buf_cpy = new char[len];
	memcpy(buf_cpy, buf, len);

	// libuv copies the uvbuf internally in uv_write,
	// so no need to keep it alive
	uv_buf_t uvbuf{buf_cpy, len};

	uv_write(
		req, this->get_stream(), &uvbuf, 1,
		[] (uv_write_t *req, int status) {
			if (status != 0) {
				LOG_WARN("[satellite] Failed to send on tcp: " + std::string(uv_strerror(status)));
			}

			// this is probably a very dirty hack.
			// but we need to free the damn buffer we created above.
			delete[] req->bufsml[0].base;

			// delete the request we also created above.
			delete req;
		}
	);
}


void TCPClient::connection_lost(int code) {
	if (code == UV_EOF) {
		LOG_INFO("[client] TCP connection closed");
	} else {
		LOG_INFO("[client] TCP connection lost");
	}

	this->close();
}


void TCPClient::close() {
	uv_read_stop(this->get_stream());

	if (not uv_is_closing(this->get_handle())) {
		uv_close(
			this->get_handle(),
			[] (uv_handle_t *handle) {
				TCPClient *this_ = (TCPClient *) handle->data;
				this_->closed();
			}
		);
	}
}


uv_stream_t *TCPClient::get_stream() {
	return reinterpret_cast<uv_stream_t *>(&this->connection);
}

uv_handle_t *TCPClient::get_handle() {
	return reinterpret_cast<uv_handle_t *>(&this->connection);
}

} // horst
