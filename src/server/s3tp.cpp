#include "../logger.h"
#include "s3tp.h"

namespace horst {

	S3TPServer::S3TPServer(Satellite *sat) : S3tpCallback(), Client(sat), channel(NULL) {
		s3tpSocketPath = (char*) S3TP_SOCKETPATH;

		// Create channel instance and set default config
		this->s3tp_cfg.port = S3TP_DEFAULT_PORT; // default Local port to bind to
		this->s3tp_cfg.options = 0;
		this->s3tp_cfg.channel = 3; // This represents the virtual channel used by NanoLink
	}

	S3TPServer::~S3TPServer() {
		uv_poll_stop(&this->connection);
	}

	void S3TPServer::on_s3tp_event(uv_poll_t *handle, int, int events) {
		S3TPServer *s3tp_link_ref = ((S3TPServer *)handle->data);

		if (s3tp_link_ref->channel != nullptr && events & UV_READABLE) {
		    s3tp_link_ref->channel->handleIncomingData();
		}
		if (s3tp_link_ref->channel != nullptr && events & UV_WRITABLE) {
		    s3tp_link_ref->channel->handleOutgoingData();
		}

		if (s3tp_link_ref->channel != nullptr)
		    s3tp_link_ref->update_events();
	}

	void S3TPServer::update_events() {
		int next_events = 0;
		int current_events = this->channel->getActiveEvents();
		if (current_events & S3TP_ASYNC_EVENT_READ)
			next_events |= UV_READABLE;
		if (current_events & S3TP_ASYNC_EVENT_WRITE)
			next_events |= UV_WRITABLE;

		uv_poll_start(&this->connection,
		              next_events | UV_DISCONNECT,
		              &S3TPServer::on_s3tp_event);
	}

	bool S3TPServer::reconnect() {
		int r = 0;
		int error = 0;
		int current_events = 0;
		int s3tp_fd;

		// Are we connected already?
		if (this->channel != NULL) {
			return true;
		}

		// Try to connect
		LOG_INFO("[s3tp] Try to reconnect...");
		this->channel = new S3tpChannelEvent(this->s3tp_cfg, *this);

		//Bind channel to S3TP daemon
		this->channel->bind(error);
		if (error == 0) {
			r = this->channel->accept();
			if (r < 0) {
				LOG_WARN("[s3tp] Failed to register for s3tp events: " + std::string(strerror(-r)));
				delete this->channel;
				this->channel = NULL;
			}
		} else {
			LOG_WARN("[s3tp] Failed to bind to s3tp: " + std::to_string(error));
			delete this->channel;
			this->channel = NULL;
		}

		if (this->channel == NULL) {
			// Regularly check connection and reconnect
			uv_timer_start(
				&this->timer,
				[] (uv_timer_t *handle) {
					// yes, handle is not a poll_t, but
					// we just care for its -> data member anyway.
					((S3TPServer*) handle->data)->reconnect();
				},
				1 * 1000, // time in milliseconds, try to reconnect every second
				0         // don't repeat
			);

			LOG_WARN("[s3tp] Reconnect failed...");
			return false;
		} else {
			current_events = this->channel->getActiveEvents();
			s3tp_fd = this->channel->getSocket()->getFileDescriptor();

			// initialize the s3tp fd events polling object
			uv_poll_init(this->loop, &this->connection, s3tp_fd);

			// make `this` reachable in event loop callbacks.
			this->connection.data = this;
			uv_poll_start(&this->connection,
		              UV_READABLE | UV_WRITABLE | UV_DISCONNECT,
		              &S3TPServer::on_s3tp_event);

			LOG_INFO("[s3tp] Reconnect succeeded...");
			return true;
		}
	}

	bool S3TPServer::start(uv_loop_t *loop_ref) {
		this->loop = loop_ref;

		uv_timer_init(this->loop, &this->timer);
		this->timer.data = this;

		if (reconnect()) {
			this->channel->handleIncomingData();
			this->channel->handleOutgoingData();
			return true;
		}

		return false;
	}

	void S3TPServer::onConnected(S3tpChannel&) {
		LOG_DEBUG("[s3tp] Connection is active");
	}

	void S3TPServer::onDisconnected(S3tpChannel&, int error) {
		LOG_WARN("[s3tp] S3TP disconnected with error " + std::to_string(error));
		uv_poll_stop(&this->connection);
		delete this->channel;
		this->channel = NULL;
		this->reconnect();
	}

	void S3TPServer::onDataReceived(S3tpChannel&, char *data, size_t len) {
		LOG_DEBUG("[s3tp] Received " + std::to_string(len) + " bytes ("+std::string(data)+")");
		this->data_received(data, len);
	}

	void S3TPServer::send(const char* msg, size_t len) {
		LOG_DEBUG("[s3tp] Sending " + std::to_string(len) + " bytes");
		this->channel->send((void*)msg, len);
	}

	void S3TPServer::close() {

	}

	void S3TPServer::onBufferFull(S3tpChannel&) {
		LOG_DEBUG("[s3tp] Buffer is full");
	}

	void S3TPServer::onBufferEmpty(S3tpChannel&) {
		LOG_DEBUG("[s3tp] Buffer is empty");
	}

	void S3TPServer::onError(int error) {
		LOG_WARN("[s3tp] connection closed with error " + std::to_string(error));
	}

} // horst
