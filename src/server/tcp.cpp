#include "tcp.h"

#include <iostream>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../client/tcp_client.h"
#include "../logger.h"
#include "../satellite.h"


namespace horst {

TCPServer::TCPServer(Satellite *sat)
	:
	satellite{sat} {}


int TCPServer::listen(int port) {
	int ret;

	uv_tcp_init(this->satellite->get_loop(), &this->server);

	LOG_INFO("[satellite] Listening on port " + std::to_string(port));

	// listen on tcp socket.
	sockaddr_in6 listen_addr;
	ret = uv_ip6_addr("::", port, &listen_addr);

	if (ret) {
		LOG_ERROR(7, "[satellite] Can't create listen address: " + std::string(uv_strerror(ret)));
		return 1;
	}

	ret = uv_tcp_bind(&this->server, (const sockaddr*) &listen_addr, 0);

	if (ret) {
		LOG_ERROR(8, "[satellite] Can't bind to socket: " + std::string(uv_strerror(ret)));
		return 1;
	}

	// make the satellite reachable in callbacks.
	this->server.data = this->satellite;

	// start to listen on the tcp socket
	ret = uv_listen(
		(uv_stream_t *)&this->server,
		4,  // < kernel connection queue size

		// when a new connection was received:
		[] (uv_stream_t *server, int status) {

			if (status < 0) {
				LOG_ERROR(9, "[satellite] New connection error: " + std::string(uv_strerror(status)));
				return;
			}

			LOG_INFO("[satellite] New connection received");

			Satellite *this_ = (Satellite *) server->data;
			auto client = std::make_unique<TCPClient>(this_);

			// accept the connection on the listening socket
			if (client->accept(server)) {
				id_t id = this_->add_client(std::move(client));
				Client *stored_client = this_->get_client(id);

				// when the client connection closes,
				// remove the handle to the stored client.
				stored_client->call_on_close(
					[this_, id] (Client *) {
						this_->remove_client(id);
					}
				);
			} else {
				LOG_WARN("[satellite] Failed to accept tcp client!");
			}
		}
	);

	if (ret) {
		LOG_WARN("[satellite] Listen error: " + std::string(uv_strerror(ret)));
		return 1;
	}

	return 0;
}

} // horst
