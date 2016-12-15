#pragma once

#include "ctlmessage.h"


namespace horst {

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
