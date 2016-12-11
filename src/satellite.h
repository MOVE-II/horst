#pragma once

#include <memory>
#include <queue>
#include <systemd/sd-bus.h>
#include <uv.h>
#include <vector>

#include "client/client.h"
#include "event/event.h"
#include "horst.h"
#include "state/state.h"


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
	 * Register the listening on dbus.
	 */
	int listen_dbus();

	/**
	 * Return the event loop.
	 */
	uv_loop_t *get_loop();

	/**
	 * Add this client to the list.
	 */
	void add_client(std::unique_ptr<Client> &&client);

	/**
	 * Called from all the callbacks that receive some external event.
	 */
	void on_event(std::unique_ptr<Event> &&event);

private:
	/** program launch arguments */
	const arguments &args;

	/** the event loop used */
	uv_loop_t loop;

	/** tcp server for control clients */
	uv_tcp_t server;

	/** list of control clients connected */
	std::vector<std::unique_ptr<Client>> clients;

	/**
	 * dbus bus handle
	 */
	sd_bus *bus;

	/**
	 * current state of the satellite.
	 */
	State current_state;
};

} // horst
