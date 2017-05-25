#include <s3tp/core/Logger.h>

#include "../event/req_shell_command.h"
#include "../horst.h"
#include "../satellite.h"
#include "s3tp.h"

namespace horst {

	S3TPServer::S3TPServer(int port, std::string socketpath)
		: S3tpCallback(),
		buf{std::make_unique<char[]>(this->max_buf_size)},
		buf_used{0} {

		s3tpSocketPath = strdup(socketpath.c_str());

		// Create channel instance and set default config
		this->s3tp_cfg.port = port; // default Local port to bind to
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
		int s3tp_fd;

		// Are we connected already?
		if (this->channel != NULL) {
			return true;
		}

		// Reset internal buffer state
		this->buf_used = 0;

		// Try to connect
		LOG_INFO("[s3tp] Try to reconnect...");
		this->channel = std::make_unique<S3tpChannelEvent>(this->s3tp_cfg, *this);

		//Bind channel to S3TP daemon
		this->channel->bind(error);
		if (error == 0) {
			r = this->channel->accept();
			if (r < 0) {
				LOG_WARN("[s3tp] Failed to register for s3tp events: " + std::string(strerror(-r)));
				this->channel = NULL;
			}
		} else {
			LOG_WARN("[s3tp] Failed to bind to s3tp: " + std::to_string(error));
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
			s3tp_fd = this->channel->getSocket()->getFileDescriptor();

			// initialize the s3tp fd events polling object
			uv_poll_init(this->loop, &this->connection, s3tp_fd);

			// make `this` reachable in event loop callbacks.
			this->connection.data = this;
			update_events();

			LOG_INFO("[s3tp] Reconnect succeeded...");
			return true;
		}
	}

	bool S3TPServer::start(uv_loop_t *loop_ref) {
		this->loop = loop_ref;

		uv_timer_init(this->loop, &this->timer);
		this->timer.data = this;

		return reconnect();
	}

	void S3TPServer::onConnected(S3tpChannel&) {
		LOG_DEBUG("[s3tp] Connection is active");
	}

	void S3TPServer::onDisconnected(S3tpChannel&, int error) {
		LOG_WARN("[s3tp] S3TP disconnected with error " + std::to_string(error));
		uv_poll_stop(&this->connection);
		this->channel = NULL;
		this->reconnect();
	}

	void S3TPServer::onDataReceived(S3tpChannel&, char *data, size_t len) {
		LOG_DEBUG("[s3tp] Received " + std::to_string(len) + " bytes");

		// Receive header
		if (this->expected == 0) {
			LOG_DEBUG("[s3tp] Receiving new command...");
			const size_t headersize = sizeof(size_t);
			std::memcpy(&this->expected, data, headersize);

			// Forward data pointer
			data += headersize;
			len -= headersize;
		}

		// Receive data
		if (len > 0) {
			LOG_DEBUG("[s3tp] Receiving command data...");

			if (this->buf_used + this->expected >= this->max_buf_size) {
				LOG_WARN("[s3tp] Receive buffer too full, closing...");
				this->close();
				return;
			}

			// Copy data into buffer
			size_t ncpy = std::min(this->expected, (this->max_buf_size - this->buf_used - 1));
			std::memcpy(&this->buf[this->buf_used], data, ncpy);
			this->buf[this->buf_used + ncpy] = '\0';
			this->buf_used += ncpy;

			if (this->buf_used >= this->expected) {

				std::string command(this->buf.get());
				auto cmd = std::make_unique<ShellCommandReq>(command, true);
				if (cmd.get() != nullptr) {
					// handle each command in the event handler
					cmd->call_on_complete([this] (const std::string &result) {
						this->send(result.c_str(), result.length());
					});

					// actually handle the event in the satellite state logic
					satellite->on_event(std::move(cmd));

					// immediately send back that the command was received.
					this->send("ack", 3);
				} else {
					LOG_WARN("[s3tp] Error while creating request, closing...");
					this->close();
					return;
				}

				this->buf_used = 0;
				this->buf[0] = '\0';
				this->expected = 0;
			}
		}
	}

	void S3TPServer::send(const char* msg, size_t len) {
		if (len == 0)
			return;
		LOG_DEBUG("[s3tp] Sending " + std::to_string(len) + " bytes");
		if (!this->channel) {
			LOG_WARN("[s3tp] Tried to send without open connection!");
			return;
		}

		// Send header
		this->channel->send((void*)&len, sizeof(len));

		// Send data
		this->channel->send((void*)msg, len);

		update_events();
	}

	void S3TPServer::close() {
		LOG_DEBUG("[s3tp] Connection closed internally");
		uv_poll_stop(&this->connection);
		this->channel = NULL;
		this->reconnect();
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
