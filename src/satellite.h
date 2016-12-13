#pragma once

#include <memory>
#include <queue>
#include <systemd/sd-bus.h>
#include <unordered_map>
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
	int listen_tcp(int port);

	/**
	 * Register the listening on dbus.
	 */
	int listen_dbus();

	/**
	 * Set up listening on the s3tp port.
	 */
	int listen_s3tp(int port);

	/**
	 * Return the event loop.
	 */
	uv_loop_t *get_loop();

	/**
	 * Add this client to the list.
	 */
	id_t add_client(std::unique_ptr<Client> &&client);

	/**
	 * Add this process to the list.
	 */
	id_t add_action(std::unique_ptr<Action> &&action);

	/**
	 * get a client by id.
	 */
	Client *get_client(id_t id);

	/**
	 * get a action by id.
	 */
	Action *get_action(id_t id);

	/**
	 * Purge a given action from the active map.
	 */
	void remove_action(id_t id);

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

	/** s3tp unix socket watcher */
	uv_loop_t s3tp_connection;

	/** list of control clients connected */
	std::unordered_map<id_t, std::unique_ptr<Client>> clients;

	/** list of running actions */
	std::unordered_map<id_t, std::unique_ptr<Action>> actions;

	/**
	 * counter to identify events.
	 */
	id_t next_id;

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
