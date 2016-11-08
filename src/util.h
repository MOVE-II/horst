#pragma once

namespace horst {

/**
 * Create a tcp socket, return the listening socket fd.
 */
int create_tcp_port(int port);


/**
 * Modify the given socket fd to be nonblocking.
 */
int make_socket_non_blocking(int sfd);


} // horst
