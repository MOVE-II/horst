#include "satellite.h"

#include <iostream>
#include <memory>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"
#include "util.h"


namespace horst {

Satellite::Satellite(const arguments &args)
	:
	args{args} {}


void Satellite::loop() {
	log("starting up connections...");

	// maximum event queue size
	constexpr int max_events = 128;

	// bytes to read from the socket each read call
	constexpr int chunksize = 1024;

	// start to create a listening port
	int sfd = create_tcp_port(args.port);
	if (sfd == -1)
		return;

	int s = make_socket_non_blocking(sfd);
	if (s == -1)
		return;

	s = listen(sfd, SOMAXCONN);
	if (s == -1) {
		perror("listen");
		return;
	}

	// create epoll interface
	int efd = epoll_create1(0);
	if (efd == -1) {
		perror("epoll_create");
		return;
	}

	epoll_event event;
	event.data.fd = sfd;
	event.events  = EPOLLIN | EPOLLET;
	s = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
	if (s == -1) {
		perror("epoll_ctl");
		return;
	}

	auto events = std::make_unique<epoll_event[]>(max_events);

	// main event loop
	while (true) {

		// wait forever until an fd is ready
		constexpr int timeout_millis = -1;
		int n = epoll_wait(efd, events.get(), max_events, timeout_millis);

		// process all events
		for (int i = 0; i < n; i++) {
			// error on this fd and socket not ready for reading
			if (events[i].events & (EPOLLERR | EPOLLHUP) ||
			    not (events[i].events & EPOLLIN)) {

				int failedfd = events[i].data.fd;
				std::cerr << "error on socket " << failedfd
				          << ", we'll close it"
				          << std::endl;

				close(failedfd);

				continue;
			}

			// the listening socket has new incoming connections
			else if (sfd == events[i].data.fd) {
				while (true) {
					char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

					sockaddr in_addr;
					socklen_t in_len = sizeof(in_addr);
					int infd = accept(sfd, &in_addr, &in_len);

					if (infd == -1) {
						if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {

							// all incoming connections processed.
							break;
						} else {
							perror("accept");
							break;
						}
					}

					// info about the client
					s = getnameinfo(
						&in_addr,
						in_len,
						hbuf,
						sizeof hbuf,
						sbuf,
						sizeof sbuf,
						NI_NUMERICHOST | NI_NUMERICSERV
					);

					if (s == 0) {
						std::cout << "new connection on fd="
						          << infd
						          << " host=" << hbuf
						          << " port=" << sbuf
						          << std::endl;
					}

					// enable nonblocking io on that socket
					s = make_socket_non_blocking(infd);
					if (s == -1)
						return;

					// add the socket to the epoll watch list
					// and register for "has data" event.
					event.data.fd = infd;
					event.events  = EPOLLIN | EPOLLET;
					s = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event);

					if (s == -1) {
						perror("epoll_ctl");
						return;
					}
				}

				// continue with the next event
				continue;
			}

			// data on the socket can be read.
			else {

				// running in edge-triggered mode, so we must read
				// all available data.
				// next notification is for new data only.

				bool done = false;

				while (true) {
					ssize_t count;
					char buf[chunksize];

					// read a chunk
					count = read(events[i].data.fd, buf, sizeof(buf));

					if (count == -1) {
						// EAGAIN means that we read all data.
						// -> continue to main event loop.
						if (errno != EAGAIN) {
							perror("read");
							done = true;
						}
						break;
					}
					else if (count == 0) {
						// eof = remote has closed.
						done = true;
						break;
					}

					// nao we have data!
					// TODO: do magic things with the data!
					s = write(1, buf, count);
					if (s == -1) {
						perror("write");
						return;
					}
				}

				if (done) {
					std::cout << "closed fd " << events[i].data.fd
					          << std::endl;

					// epoll removes the closed fd from the monitored list
					close(events[i].data.fd);
				}
			}
		}
	}

	close(sfd);
}

} // horst
