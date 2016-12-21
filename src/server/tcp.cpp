#include "tcp.h"

#include <iostream>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../client/tcp_client.h"
#include "../satellite.h"


namespace horst {

TCPServer::TCPServer(Satellite *sat)
	:
	satellite{sat} {}


int TCPServer::listen(int port) {
	int ret;

	uv_tcp_init(this->satellite->get_loop(), &this->server);

	std::cout << "[satellite] listening on port "
	          << port << "..." << std::endl;

	// listen on tcp socket.
	sockaddr_in6 listen_addr;
	ret = uv_ip6_addr("::", port, &listen_addr);

	if (ret) {
		std::cout << "[satellite] can't create liste addr: "
		          << uv_strerror(ret) << std::endl;
		return 1;
	}

	ret = uv_tcp_bind(&this->server, (const sockaddr*) &listen_addr, 0);

	if (ret) {
		std::cout << "[satellite] can't bind to socket: "
		          << uv_strerror(ret) << std::endl;
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
				std::cout << "[satellite] new connection error: "
				          << uv_strerror(status) << std::endl;
				return;
			}

			std::cout << "[satellite] new connection received" << std::endl;

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
			}
			else {
				std::cout << "[satellite] failed to accept tcp client"
				          << std::endl;
			}
		}
	);

	if (ret) {
		std::cout << "[satellite] listen error: "
		          << uv_strerror(ret) << std::endl;
		return 1;
	}

	return 0;
}

} // horst
