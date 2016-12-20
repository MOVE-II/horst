#pragma once

#include <uv.h>
#include <s3tp/connector/S3tpChannelEvent.h>


namespace horst {


class S3tpCallback4Horst : public S3tpCallback {
public:
	S3tpCallback4Horst();
	
	// s3tp event callbacks
	void onConnected(S3tpChannel &channel) override;
	void onDisconnected(S3tpChannel &channel,
	                      int error) override;
	void onDataReceived(S3tpChannel &channel,
	                  char *data, size_t len) override;
	void onBufferFull(S3tpChannel &channel) override;
	void onBufferEmpty(S3tpChannel &channel) override;
	void onError(int error) override;

};


class S3TPServer {
public:
	S3TPServer();

	virtual ~S3TPServer();

	static void on_s3tp_event(uv_poll_t *handle,
                          int status,
                          int events) ;
	int initiate(uint8_t port, uv_loop_t *loop_ref);

protected:
	/**
	 * s3tp connection configurations
	 */
	ClientConfig s3tp_cfg;

	/**
	 * s3tp connection handle
	 */
	S3tpChannelEvent channel;

	/**
	 * s3tp callback object
	 */
	S3tpCallback4Horst cbs;

	/**
	 * tcp server for control clients
	 */
	uv_poll_t connection;
};


} // horst
