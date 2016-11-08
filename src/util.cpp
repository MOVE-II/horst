#include "util.h"


#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


namespace horst {

int create_tcp_port(int port) {
	addrinfo hints;
	addrinfo *result, *rp;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family   = AF_UNSPEC;   // IPv4 and IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP socket
	hints.ai_flags    = AI_PASSIVE;  // any interface

	int s = getaddrinfo(nullptr,
	                    std::to_string(port).c_str(),
	                    &hints, &result);

	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}

	int sfd;

	for (rp = result; rp != nullptr; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0) {
			// bind success!
			break;
		}

		close(sfd);
	}

	if (rp == nullptr) {
		fprintf(stderr, "Failed to bind to %d\n", port);
		return -1;
	}

	freeaddrinfo(result);

	return sfd;
}

int make_socket_non_blocking(int sfd) {
	int flags, s;

	// get current flags
	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		return -1;
	}

	// add the nonblock flag
	flags |= O_NONBLOCK;
	s = fcntl(sfd, F_SETFL, flags);
	if (s == -1) {
		perror("fcntl");
		return -1;
	}

	return 0;
}

} // horst
