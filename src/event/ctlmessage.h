#pragma once

#include <memory>
#include <string>

#include "event.h"


namespace horst {

/**
 * Control messages via s3tp from earth.
 */
class ControlMessage : public Event {
public:
	ControlMessage();
	virtual ~ControlMessage() = default;

	/**
	 * Parse the given command and return the appropriate
	 * control message object.
	 */
	static std::unique_ptr<ControlMessage> parse(const std::string &msg);

	/**
	 * control message is never a fact.
	 * @returns false
	 */
	bool is_fact() const override;
};


/**
 * Control message to call a procedure.
 * Those are mainly some maneuvers the satellite supports.
 */
class ProcedureCallReq : public ControlMessage {
public:
	ProcedureCallReq(const std::string &name);

	void update(State &state) override;

private:
	std::string name;
};


/**
 * This is just a shell command executed directly on the satellite.
 */
class ShellCommandReq : public ControlMessage {
public:
	ShellCommandReq(const std::string &command);

	void update(State &state) override;

private:
	std::string cmd;
};


/**
 * daemon control commands
 */
enum class daemon_ctl_action_t {
	START,
	STOP,
	RESTART,
	STATUS
};


/**
 * Control systemd services.
 */
class DaemonControlReq : public ControlMessage {
public:
	DaemonControlReq(daemon_ctl_action_t action,
	                 const std::string &service_name);

	void update(State &state) override;
private:

	daemon_ctl_action_t action;
	std::string service_name;
};

} // horst
