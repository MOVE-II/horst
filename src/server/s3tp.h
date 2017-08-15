#pragma once

#include <s3tp/connector/S3tpChannelEvent.h>
#include <uv.h>

#include "../process.h"


namespace horst {


class S3TPServer : public S3tpCallback {

public:
	S3TPServer(int, std::string);
	virtual ~S3TPServer();

	static void on_s3tp_event(uv_poll_t*, int, int);

	/**
	 * Start S3TP server
	 */
	bool start(uv_loop_t*);

	/**
	 * Send some data downlink
	 */
	void send(const char*, uint32_t, bool, bool);

private:
	/**
	 * S3TP connection configurations
	 */
	ClientConfig s3tp_cfg;

	/**
	 * S3TP connection handle
	 */
	std::unique_ptr<S3tpChannelEvent> channel;

	/**
	 * Buffer for received data
	 */
	std::vector<char> buf;

	/**
	 * Number of bytes expected for command
	 */
	uint32_t expected;

	/**
	 * Reference to event loop
	 */
	uv_loop_t *loop;

	/**
	 * S3TP connection
	 */
	uv_poll_t connection;

	/**
	 * Timer for automatic reconnect
	 */
	uv_timer_t timer;

	/**
	 * Currently running process for user shell command
	 */
	std::unique_ptr<Process> process;

	/**
	 * Output buffer
	 */
	std::vector<char> outbuf;

	void update_events();

	/**
	 * Try to send buffered data
	 * Returns true, if successful or fatale error, returns false, if data
	 * could not be sent and sending needs to be retried later
	 */
	bool send_buf();

	/**
	 * If not connected, try to re-establish
	 */
	bool reconnect();

	/**
	 * Returns true when buf contains the string in the message payload
	 */
	bool compare_to_buf(const std::string& str);

	// S3TP event callbacks
	void onConnected(S3tpChannel &channel) override;
	void onDisconnected(S3tpChannel &channel, int error) override;
	void onDataReceived(S3tpChannel &channel, char *data, size_t len) override;
	void onBufferFull(S3tpChannel &channel) override;
	void onBufferEmpty(S3tpChannel &channel) override;
	void onError(int error) override;

	void close();
};


} // horst
