#pragma once

#include <s3tp/connector/S3tpChannelEvent.h>
#include <uv.h>

#include "../client/client.h"


namespace horst {

class S3TPServer : public S3tpCallback, public Client {
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
	void send(const char*, size_t) override;
	void close() override;
};


} // horst
