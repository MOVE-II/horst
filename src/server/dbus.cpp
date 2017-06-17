#include "dbus.h"

#include <iostream>
#include <stdexcept>

#include "../event/eps_signal.h"
#include "../event/leop_req.h"
#include "../event/adcs_signal.h"
#include "../event/adcs_req_signal.h"
#include "../event/maneuvermode_signal.h"
#include "../event/manualmode_req.h"
#include "../event/payload_req.h"
#include "../event/payload_signal.h"
#include "../event/req_shell_command.h"
#include "../event/safemode_req.h"
#include "../event/thm_signal.h"
#include "../satellite.h"
#include "../state/thm.h"
#include "../util.h"


namespace horst {

void DBusConnection::handle_dbus() {
	sd_bus *bus = this->get_bus();

	// let dbus handle the requests available
	while (true) {
		int r = sd_bus_process(bus, nullptr);
		if (r < 0) {
			LOG_WARN("[dbus] Failed to process bus: : " + std::string(strerror(-r)));
			break;
		}
		else if (r > 0) {
			// try to process another request!
			continue;
		}
		else {
			// no more progress, wait for the next callback.
			break;
		}
	}

	// update the events we watch for on the socket.
	this->update_events();
}


DBusConnection::DBusConnection(Satellite *sat)
	:
	satellite{sat},
	loop{sat->get_loop()},
	bus{nullptr},
	bus_slot{nullptr} { }


DBusConnection::~DBusConnection() {}


void DBusConnection::update_events() {
	sd_bus *bus = this->get_bus();

	// prepare the callback for calling us the next time.
	int new_events = util::poll_to_libuv_events(
		sd_bus_get_events(bus)
	);

	uint64_t usec;
	int r = sd_bus_get_timeout(bus, &usec);

	if (not r) {
		// if the timer is running already, it is stopped automatically
		// inside uv_timer_start.
		uv_timer_start(
			&this->timer,
			[] (uv_timer_t *handle) {
				// yes, handle is not a poll_t, but
				// we just care for its -> data member anyway.
				((DBusConnection*) handle->data)->handle_dbus();
			},
			usec / 1000, // time in milliseconds, sd_bus provides µseconds
			0            // don't repeat
		);
	}

	// always watch for disconnects:
	new_events |= UV_DISCONNECT;

	// activate the socket watching and if active, handle dbus
	uv_poll_start(&this->connection, new_events, [](uv_poll_t *handle, int, int) {
	    ((DBusConnection*) handle->data)->handle_dbus();
	});
}


static int dbus_exec(sd_bus_message *m, void *userdata, sd_bus_error*) {

	DBusConnection *this_ = (DBusConnection *)userdata;
	const char *command;
	int r = sd_bus_message_read(m, "s", &command);
	if (r < 0) {
		LOG_ERROR(3, "[dbus] exec() failed to parse parameters: " + std::string(strerror(-r)));
		return r;
	}

	LOG_INFO("[dbus] Exec command: " + std::string(command));
	auto req = std::make_shared<ShellCommandReq>(command);
	this_->get_sat()->on_event(std::move(req));

	// TODO: maybe do req->call_on_complete([]{real_dbus_return()})

	return sd_bus_reply_method_return(m, "x", 0);
}

static int dbus_safemode(sd_bus_message *m, void *userdata, sd_bus_error*) {
	DBusConnection *this_ = (DBusConnection *)userdata;
	char* safemode;
	int r = sd_bus_message_read(m, "s", &safemode);
	if (r < 0) {
		LOG_ERROR(3, "[dbus] safemode() failed to parse parameters: " + std::string(strerror(-r)));
		return sd_bus_reply_method_return(m, "b", false);
	}

	LOG_INFO("[dbus] Request to set safemode to " + std::string(safemode));
	bool bsafe = false;
	try {
		bsafe = State::str2bool(safemode);
	} catch (const std::invalid_argument&) {
		return sd_bus_reply_method_return(m, "b", false);
	}
	auto req = std::make_shared<SafeModeReq>(bsafe ? 3 : 0);
	this_->get_sat()->on_event(std::move(req));

	return sd_bus_reply_method_return(m, "b", true);
}

static int dbus_maneuvermode(sd_bus_message *m, void *userdata, sd_bus_error*) {
	DBusConnection *this_ = (DBusConnection *)userdata;
	char* maneuvermode;
	int r = sd_bus_message_read(m, "s", &maneuvermode);
	if (r < 0) {
		LOG_ERROR(3, "[dbus] maneuvermode() failed to parse parameters: " + std::string(strerror(-r)));
		return sd_bus_reply_method_return(m, "b", false);
	}

	LOG_INFO("[dbus] Request to set maneuvermode to " + std::string(maneuvermode));
	bool bmaneuver = false;
	try {
		bmaneuver = State::str2bool(maneuvermode);
	} catch (const std::invalid_argument&) {
		return sd_bus_reply_method_return(m, "b", false);
	}
	auto req = std::make_shared<ManeuverModeSignal>(bmaneuver);
	this_->get_sat()->on_event(std::move(req));

	return sd_bus_reply_method_return(m, "b", true);
}

static int getBeaconData(sd_bus_message *m, void *userdata, sd_bus_error*) {
	DBusConnection *this_ = (DBusConnection *)userdata;
	State* state = this_->get_sat()->get_state();
	std::vector<uint8_t> data;
	sd_bus_message *retm;

	// Create new return message
	int r = sd_bus_message_new_method_return(m, &retm);
	if (r < 0) {
		std::cout << "Failed to create return message: " << strerror(-r) << std::endl;
		return 1;
	}

	// Fill data
	data.push_back((uint8_t) state->safemode);
	data.push_back((uint8_t) state->manualmode);
	data.push_back((uint8_t) state->maneuvermode);
	data.push_back((uint8_t) (state->eps.battery_level & 0xFF));
	data.push_back((uint8_t) (state->eps.battery_level >> 8));
	data.push_back((uint8_t) state->thm.all_temp);
	data.push_back((uint8_t) state->adcs.pointing);
	data.push_back((uint8_t) state->adcs.requested);
	data.push_back((uint8_t) state->pl.daemon);
	data.push_back((uint8_t) state->leop);

	// Append data to message
	r = sd_bus_message_append_array(retm, 'y', &data[0], data.size());
	if (r < 0) {
		std::cout << "Failed to append data to message: " << strerror(-r) << std::endl;
		return 1;
	}

	// Send message on bus
	r = sd_bus_send(sd_bus_message_get_bus(m), retm, NULL);
	if (r < 0) {
		std::cout << "Failed to reply return message: " << strerror(-r) << std::endl;
		return 1;
	}

	sd_bus_message_unref(retm);
	return 1;
}

static int dbus_manualmode(sd_bus_message *m, void *userdata, sd_bus_error*) {
	DBusConnection *this_ = (DBusConnection *)userdata;
	char* manualmode;
	int r = sd_bus_message_read(m, "s", &manualmode);
	if (r < 0) {
		LOG_ERROR(3, "[dbus] manualmode() failed to parse parameters: " + std::string(strerror(-r)));
		return sd_bus_reply_method_return(m, "b", false);
	}

	LOG_INFO("[dbus] Request to set manualmode to " + std::string(manualmode));
	bool bmanual = false;
	try {
		bmanual = State::str2bool(manualmode);
	} catch (const std::invalid_argument&) {
		return sd_bus_reply_method_return(m, "b", false);
	}
	auto req = std::make_shared<ManualModeReq>(bmanual);
	this_->get_sat()->on_event(std::move(req));

	return sd_bus_reply_method_return(m, "b", true);
}

static int
checkDaemon(sd_bus_message *m, void*, sd_bus_error*) {
    LOG_DEBUG("Daemons checkDaemon is called");
    return sd_bus_reply_method_return(m, "q", 0);
}

static int
getVersion(sd_bus_message *m, void*, sd_bus_error*) {
    sd_bus_message *retm;
    LOG_DEBUG("Daemon is asked for version");
    std::vector<uint16_t> data;

    // Create new return message
    int r = sd_bus_message_new_method_return(m, &retm);
    if (r < 0) {
	LOG_WARN(std::string("Failed to create return message: " + std::string(strerror(-r))));
	return 1;
    }

    // Append data to message
    r = sd_bus_message_append(retm, "s", std::string("daemon=" + std::string(VERSION)).c_str());
    if (r < 0) {
	LOG_WARN(std::string("Failed to append data to message: " + std::string(strerror(-r))));
	return 1;
    }

    // Send message on bus
    r = sd_bus_send(sd_bus_message_get_bus(m), retm, NULL);
    if (r < 0) {
	LOG_WARN(std::string("Failed to reply return message: " + std::string(strerror(-r))));
	return 1;
    }

    sd_bus_message_unref(retm);
    return 1;
}

static int
checkHardware(sd_bus_message *m, void*, sd_bus_error*) {
    LOG_DEBUG("Daemons checkHardware is called");
    return sd_bus_reply_method_return(m, "q", 0);
}

static const sd_bus_vtable horst_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("exec", "s", "x", dbus_exec, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("setSafemode", "s", "b", dbus_safemode, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("setManeuvermode", "s", "b", dbus_maneuvermode, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("setManualmode", "s", "b", dbus_manualmode, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("getBeaconData", "", "ay", getBeaconData, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("checkDaemon", "", "q", checkDaemon, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("checkHardware", "", "q", checkHardware, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("getVersion", "", "s", getVersion, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_SIGNAL("adcsStateReached", "s", 0),
	SD_BUS_SIGNAL("payloadConditionsFulfilled", "", 0),
	SD_BUS_SIGNAL("payloadMeasurementDone", "", 0),
	SD_BUS_SIGNAL("leopStateChange", "s", 0),
	SD_BUS_SIGNAL("epsChargeStateChange", "q", 0),
	SD_BUS_SIGNAL("thmStateChange", "s", 0),
	SD_BUS_VTABLE_END
};


int DBusConnection::connect() {
	int r = sd_bus_open_system(&this->bus);

	if (r < 0) {
		LOG_ERROR(4, "[dbus] Failed to connect to bus: " + std::string(strerror(-r)));
		goto clean_return;
	}

	// install the horst object
	r = sd_bus_add_object_vtable(
		this->bus,
		&this->bus_slot,
		"/moveii/horst",  // object path
		"moveii.horst",   // interface name
		horst_vtable,
		this              // this is the userdata that'll be passed
		                  // to the dbus methods
	);

	if (r < 0) {
		LOG_ERROR(7, "[dbus] Failed to install the horst sdbus object: " + std::string(strerror(-r)));
		goto clean_return;
	}

	// register our service name
	r = sd_bus_request_name(this->bus, "moveii.horst", 0);
	if (r < 0) {
		LOG_ERROR(5, "[dbus] Failed to acquire service name: " + std::string(strerror(-r)));
		goto clean_return;
	}

	// register the filedescriptor from
	// sd_bus_get_fd(bus) to libuv
	uv_poll_init(this->loop, &this->connection, sd_bus_get_fd(this->bus));


	// make `this` reachable in callbacks.
	this->connection.data = this;

	// init the dbus-event-timer
	uv_timer_init(this->loop, &this->timer);
	this->timer.data = this;

	// register which signals are interesting for horst
	this->watch_for_signals();

	// process initial events and set up the
	// events and timers for subsequent calls
	this->handle_dbus();

	LOG_DEBUG("[dbus] Listener initialized");
	return 0;

clean_return:
	sd_bus_slot_unref(this->bus_slot);
	sd_bus_unref(this->bus);
	this->bus = nullptr;

	return 1;
}


int DBusConnection::close() {
	// TODO: maybe this memoryerrors when the loop actually
	//       does the cleanup. we have to wait for the callback.
	uv_close((uv_handle_t *) &this->timer, nullptr);

	uv_poll_stop(&this->connection);

	sd_bus_close(this->bus);
	sd_bus_slot_unref(this->bus_slot);
	sd_bus_unref(this->bus);
	return 0;
}


Satellite *DBusConnection::get_sat() {
	return this->satellite;
}


sd_bus *DBusConnection::get_bus() {
	return this->bus;
}


void DBusConnection::watch_for_signals() {
	int r;

	// set up watched signals
	// https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-routing-match-rules

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"interface='moveii.thm',"
		"member='thmStateChange'",
		[] (sd_bus_message* m, void *userdata, sd_bus_error*) -> int {
			DBusConnection *this_ = (DBusConnection *) userdata;
			char* thmlevel;
			int r;

			r = sd_bus_message_read(m, "s", &thmlevel);
			if (r < 0) {
				LOG_WARN("[dbus] Failed to receive THM state change: " + std::string(strerror(-r)));
				return 0;
			}
			LOG_INFO("[dbus] THM state changed to: " + std::string(thmlevel));

			/* Generate fact and send it to state logic */
			auto req = std::make_shared<THMSignal>(THM::str2temp(thmlevel));
			this_->get_sat()->on_event(std::move(req));

			return 0;
		},
		this
	);
	if (r < 0) {
		LOG_ERROR(6, "[dbus] Failed to add temperature level x match: " + std::string(strerror(-r)));
	}

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"interface='moveii.eps',"
		"member='epsChargeStateChange'",
		[] (sd_bus_message* m, void *userdata, sd_bus_error*) -> int {
			DBusConnection *this_ = (DBusConnection *) userdata;
			uint16_t bat;
			int r;

			r = sd_bus_message_read(m, "q", &bat);
			if (r < 0) {
				LOG_WARN("[dbus] Failed to receive EPS battery state change: " + std::string(strerror(-r)));
				return 0;
			}
			LOG_INFO("[dbus] EPS battery state changed to: " + std::to_string(bat));

			/* Generate fact and send it to state logic */
			auto req = std::make_shared<EPSSignal>(bat);
			this_->get_sat()->on_event(std::move(req));

			return 0;
		},
		this
	);
	if (r < 0) {
		LOG_ERROR(6, "[dbus] Failed to add EPS battery level x match: " + std::string(strerror(-r)));
	}

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"interface='moveii.leop',"
		"member='leopStateChange'",
		[] (sd_bus_message* m, void *userdata, sd_bus_error*) -> int {
			DBusConnection *this_ = (DBusConnection *) userdata;
			char* leop;
			int r;

			r = sd_bus_message_read(m, "s", &leop);
			if (r < 0) {
				LOG_WARN("[dbus] Failed to receive LEOP state change: " + std::string(strerror(-r)));
				return 0;
			}
			LOG_INFO("[dbus] LEOP state changed to: " + std::string(leop));

			/* Generate fact and send it to state logic */
			auto sig = std::make_shared<LEOPReq>(State::str2leop(leop));
			this_->get_sat()->on_event(std::move(sig));

			return 0;
		},
		this
	);
	if (r < 0) {
		LOG_ERROR(6, "[dbus] Failed to add LEOP state change match: " + std::string(strerror(-r)));
	}

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"interface='moveii.pl',"
		"member='payloadConditionsNotFulfilled'",
		[] (sd_bus_message*, void *userdata, sd_bus_error*) -> int {
			DBusConnection *this_ = (DBusConnection *) userdata;
			LOG_INFO("[dbus] Payload signaled measurement conditions not fulfilled");

			/* Generate fact and send it to state logic */
			auto sig = std::make_shared<PayloadSignal>(Payload::daemon_state::IDLE);
			this_->get_sat()->on_event(std::move(sig));

			return 0;
		},
		this
	);
	if (r < 0) {
		LOG_ERROR(6, "[dbus] Failed to add payload measurement conditions not fullfilled match: " + std::string(strerror(-r)));
	}

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"interface='moveii.pl',"
		"member='payloadMeasurementDone'",
		[] (sd_bus_message*, void *userdata, sd_bus_error*) -> int {
			DBusConnection *this_ = (DBusConnection *) userdata;
			LOG_INFO("[dbus] Payload measurement is done");

			/* Generate fact and send it to state logic */
			auto sig = std::make_shared<PayloadSignal>(Payload::daemon_state::IDLE);
			this_->get_sat()->on_event(std::move(sig));

			return 0;
		},
		this
	);
	if (r < 0) {
		LOG_ERROR(6, "[dbus] Failed to add payload measurement done match: " + std::string(strerror(-r)));
	}

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"interface='moveii.pl',"
		"member='payloadConditionsFulfilled'",
		[] (sd_bus_message*, void *userdata, sd_bus_error*) -> int {
			DBusConnection *this_ = (DBusConnection *) userdata;
			LOG_INFO("[dbus] Payload wants to measure");

			/* Generate fact and send it to state logic */
			auto sig = std::make_shared<PayloadReq>(Payload::daemon_state::WANTMEASURE);
			this_->get_sat()->on_event(std::move(sig));

			return 0;
		},
		this
	);
	if (r < 0) {
		LOG_ERROR(6, "[dbus] Failed to add payload conditions fulfilled match: " + std::string(strerror(-r)));

	}

	r = sd_bus_add_match(
		this->bus,
		nullptr,
		"type='signal',"
		"interface='moveii.adcs',"
		"member='adcsStateReached'",
		[] (sd_bus_message* m, void *userdata, sd_bus_error*) -> int {
			DBusConnection *this_ = (DBusConnection *) userdata;
			char* adcs_status_string;
			int r;

			r = sd_bus_message_read(m, "s", &adcs_status_string);
			if (r < 0) {
				LOG_WARN("[dbus] Failed to receive ADCS state change: " + std::string(strerror(-r)));
				return 0;
			}
			LOG_INFO("[dbus] ADCS state reached: " + std::string(adcs_status_string));

			/* Generate fact and send it to state logic */
			auto sig = std::make_shared<ADCSSignal>(ADCS::str2state(adcs_status_string));
			this_->get_sat()->on_event(std::move(sig));

			return 0;
		},
		this
	);
	if (r < 0) {
		LOG_ERROR(6, "[dbus] Failed to add ADCS state change match: " + std::string(strerror(-r)));
	}
}

}  // horst
