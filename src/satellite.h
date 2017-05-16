#pragma once

#include <memory>
#include <s3tp/core/Logger.h>
#include <unordered_map>
#include <uv.h>

#include "event/event.h"
#include "horst.h"
#include "id.h"
#include "server/dbus.h"
#include "server/s3tp.h"
#include "state/state.h"


namespace horst {


class Satellite {
public:
	/**
	 * Create the satellite controller.
	 * Pass its configuration.
	 */
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
	 * Register the listening on dbus.
	 */
	int listen_dbus();

	/**
	 * Return the event loop.
	 */
	uv_loop_t *get_loop();

	/**
	 * Add this process to the list.
	 */
	id_t add_action(std::unique_ptr<Action> &&action);

	/**
	 * get a action by id.
	 */
	Action *get_action(id_t id);

	/**
	 * Get path where to find action scripts
	 */
	std::string get_scripts_path();

	/**
	 * Get current state of satellite
	 */
	State *get_state();

	/**
	 * Purge a given action from the active map.
	 */
	void remove_action(id_t id);

	/**
	 * Called from all the callbacks that receive some external event.
	 */
	void on_event(std::shared_ptr<Event> &&event);

private:
	/** program launch arguments */
	const arguments &args;

	/** the event loop used */
	uv_loop_t loop;

	/** s3tp unix socket watcher */
	S3TPServer s3tp_link;

	/** DBus connection */
	DBusConnection dbus;

	/** list of running actions */
	std::unordered_map<id_t, std::unique_ptr<Action>> actions;

	/**
	 * counter to identify events.
	 */
	id_t next_id;

	/**
	 * current state of the satellite.
	 */
	State current_state;
};

} // horst
