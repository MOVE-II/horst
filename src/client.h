#pragma once

#include <uv.h>

#include <memory>

namespace horst {

class Satellite;

class Client {
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

	// close the connection
	void close();

private:
	/** close the stream when destructing */
	bool close_stream;

	/** connection stream */
	std::unique_ptr<uv_tcp_t> connection;

	/** server associated to the client */
	uv_stream_t *server;
};

} // horst
