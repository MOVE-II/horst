#pragma once

#include <s3tp/connector/S3tpChannelEvent.h>
#include <uv.h>

#include "../client/client.h"
#include "s3tp_protocol.h"


namespace horst {

class S3TPServer : public S3tpCallback {
	static constexpr size_t max_buf_size = 4096;

public:
	S3TPServer(Satellite*, int, std::string);
	virtual ~S3TPServer();

	static void on_s3tp_event(uv_poll_t*, int, int);

	/**
	 * Start s3tp server
	 */
	bool start(uv_loop_t*);

private:
	/**
	 * s3tp connection configurations
	 */
	ClientConfig s3tp_cfg;

	/**
	 * s3tp connection handle
	 */
	std::unique_ptr<S3tpChannelEvent> channel;

	/**
	 * Buffer for received data
	 */
	std::unique_ptr<char[]> buf;

	/**
	 * Number of bytes in buffer used
	 */
	size_t buf_used;

	/**
	 * Protocol header of currently buffered incoming message
	 */
	s3tp_horst_header header;

	/**
	 * Reference to event loop
	 */
	uv_loop_t *loop;

	/**
	 * tcp server for control clients
	 */
	uv_poll_t connection;

	/**
	 * Timer for automatic reconnect
	 */
	uv_timer_t timer;

	void update_events();

	/**
	 * If not connected, try to re-establish
	 */
	bool reconnect();

	// s3tp event callbacks
	void onConnected(S3tpChannel &channel) override;
	void onDisconnected(S3tpChannel &channel, int error) override;
	void onDataReceived(S3tpChannel &channel, char *data, size_t len) override;
	void onBufferFull(S3tpChannel &channel) override;
	void onBufferEmpty(S3tpChannel &channel) override;
	void onError(int error) override;

	// Client methods
	void send(const char*, size_t);
	void close();
};


} // horst
