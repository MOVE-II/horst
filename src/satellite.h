#pragma once

#include <memory>
#include <uv.h>
#include <vector>

#include "client.h"
#include "daemon.h"
#include "horst.h"


namespace horst {

class Satellite {
public:
	Satellite(const arguments &args);
	virtual ~Satellite() = default;

	/**
	 * Launch the satellite state processing.
	 *
	 * @returns program exit code
	 */
	int run();

	/**
	 * Return the event loop.
	 */
	uv_loop_t *get_loop();

	/**
	 * Add this client to the list.
	 */
	void add_client(Client &&client);

private:
	const arguments &args;

	uv_loop_t loop;
	uv_tcp_t server;

	std::vector<Client> clients;

	Daemon eps;
	Daemon com;
	Daemon adcs;
};

}
