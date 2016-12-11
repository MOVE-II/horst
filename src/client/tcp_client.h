#pragma once

#include <uv.h>


#include "client.h"


namespace horst {

class TCPClient : public Client {
public:
	TCPClient(Satellite *satellite);

	TCPClient(TCPClient &&other);
	TCPClient &operator =(TCPClient &&other);

	virtual ~TCPClient() override;

	/**
	 * Accept the connection from the event loop.
	 */
	bool accept(uv_stream_t *server);

	/** register receiving callbacks to the event loop */
	void start_receiving();

	/**
	 * Send data to the client.
	 */
	void send(const char *buf, size_t len) override;

	/**
	 * called when the connection gets lost.
	 * code == UV_EOF when it's EOF.
	 */
	void connection_lost(int code);

	uv_stream_t *get_stream();
	uv_handle_t *get_handle();

	/** close the connection to the client */
	void close() override;

protected:
	/** connection stream */
	std::unique_ptr<uv_tcp_t> connection;

	/** server associated to the client */
	uv_stream_t *server;
};

} // horst
