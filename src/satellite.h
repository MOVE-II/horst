#pragma once

#include <memory>
#include <queue>
#include <uv.h>
#include <vector>

#include "client.h"
#include "ctlmessage.h"
#include "daemon.h"
#include "horst.h"


namespace horst {

class Satellite {
public:
	Satellite(const arguments &args);
	Satellite(Satellite &&) = delete;
	Satellite(const Satellite &) = delete;
	Satellite& operator =(Satellite &&) = delete;
	Satellite& operator =(const Satellite &) = delete;
	virtual ~Satellite();

	/**
	 * Launch the satellite state processing.
	 *
	 * @returns program exit code
	 */
	int run();

	/**
	 * Set up listening on the given TCP port for
	 * debugging clients.
	 */
	int listen_tcp(int port, uv_tcp_t *server);

	/**
	 * Return the event loop.
	 */
	uv_loop_t *get_loop();

	/**
	 * Add this client to the list.
	 */
	void add_client(Client &&client);

	/**
	 * Enqueue a new control message to be handled.
	 */
	void enqueue(ControlMessage &&msg);

private:
	const arguments &args;

	uv_loop_t loop;
	uv_tcp_t server;

	std::vector<Client> clients;

	std::queue<ControlMessage> commands;

	Daemon eps;
	Daemon com;
	Daemon adcs;
};

}
