#pragma once

#include <uv.h>

namespace horst {

class Satellite;

class TCPServer {
public:
	TCPServer(Satellite *sat);

	virtual ~TCPServer() = default;

	int listen(int port);

protected:
	Satellite *satellite;

	/** tcp server for control clients */
	uv_tcp_t server;
};

} // horst
