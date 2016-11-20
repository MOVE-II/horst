#pragma once

#include <uv.h>

#include <memory>

namespace horst {

class Satellite;

class Client {
	static constexpr size_t max_buf_size = 4096;

public:
	Client(Satellite *satellite);

	Client(Client &&other);
	Client &operator =(Client &&other);

	// no copies
	Client(const Client &other) = delete;
	Client &operator =(const Client &other) = delete;

	virtual ~Client();

	bool accept(uv_stream_t *server);
	void register_callbacks();

	uv_stream_t *get_stream();
	uv_handle_t *get_handle();

	/** called when this client receives data */
	void data_received(const char *data, size_t size);

	/**
	 * called when the connection gets lost.
	 * code == UV_EOF when it's EOF.
	 */
	void connection_lost(int code);

	/** close the connection */
	void close();

private:
	/** Satellite the client connected to */
	Satellite *satellite;

	/** close the stream when destructing */
	bool close_stream;

	/** connection stream */
	std::unique_ptr<uv_tcp_t> connection;

	/** server associated to the client */
	uv_stream_t *server;

	/** buffer for input commands */
	std::unique_ptr<char[]> buf;

	/** position in the read buffer */
	size_t buf_used;
};

} // horst
