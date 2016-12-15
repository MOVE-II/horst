#include "req_daemon_control.h"

#include <sstream>
#include <tuple>

#include "../state/state.h"


namespace horst {

DaemonControlReq::DaemonControlReq(daemon_ctl_action_t action,
                                   const std::string &service_name)
	:
	action{action},
	service_name{service_name} {}


void DaemonControlReq::update(State &state) {
	std::stringstream command;
	command << "sudo systemctl";

	switch (this->action) {
	case daemon_ctl_action_t::START:
		command << "start";
		break;
	case daemon_ctl_action_t::STOP:
		command << "stop";
		break;
	case daemon_ctl_action_t::RESTART:
		command << "restart";
		break;
	case daemon_ctl_action_t::STATUS:
		command << "status";
		break;
	}

	command << this->service_name;

	state.computer.shell_commands.push_back(
		std::make_tuple(
			command.str(),
			std::static_pointer_cast<ControlMessage>(this->shared_from_this())
		)
	);
}

} // horst
