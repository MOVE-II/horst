#pragma once

#include <systemd/sd-bus.h>
#include <uv.h>


namespace horst {

class Satellite;


class DBusConnection {
public:
	DBusConnection(Satellite *sat);

	virtual ~DBusConnection();

	/** connect to dbus */
	int connect();

	/** update the events watched for on the filedescriptor */
	void update_events();

	/** close the connections */
	int close();

	/**
	 * Get the satellite the dbus belongs to.
	 */
	Satellite *get_sat();

	/**
	 * Return the bus handle.
	 */
	sd_bus *get_bus();

protected:
	Satellite *satellite;
	uv_loop_t *loop;

	/**
	 * polling object for dbus events
	 */
	uv_poll_t connection;

	/**
	 * dbus also wants to be called periodically
	 */
	uv_timer_t timer;

	/**
	 * dbus bus handle
	 */
	sd_bus *bus;

	/**
	 * dbus slot handle
	 */
	sd_bus_slot *bus_slot;

};

} // horst
