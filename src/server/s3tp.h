#pragma once

#include <uv.h>
#include <s3tp/connector/S3tpChannelEvent.h>

#include "../client/client.h"

#define S3TP_DEFAULT_PORT 99
#define S3TP_SOCKETPATH "/tmp/s3tp4000"


namespace horst {

class S3TPServer : public S3tpCallback, public Client {
public:
	S3TPServer(Satellite*);

	virtual ~S3TPServer();

	static void on_s3tp_event(uv_poll_t *handle, int status, int events);

	/**
	 * Start s3tp server
	 */
	int start(uv_loop_t*);

protected:
	/**
	 * s3tp connection configurations
	 */
	ClientConfig s3tp_cfg;

	/**
	 * s3tp connection handle
	 */
	S3tpChannelEvent *channel;

	/**
	 * Reference to event loop
	 */
	uv_loop_t *loop;

	/**
	 * tcp server for control clients
	 */
	uv_poll_t connection;

	void update_events();

	// s3tp event callbacks
	void onConnected(S3tpChannel &channel) override;
	void onDisconnected(S3tpChannel &channel, int error) override;
	void onDataReceived(S3tpChannel &channel, char *data, size_t len) override;
	void onBufferFull(S3tpChannel &channel) override;
	void onBufferEmpty(S3tpChannel &channel) override;
	void onError(int error) override;

	// Client methods
	void send(const char*, size_t) override;
	void close() override;
};


} // horst
