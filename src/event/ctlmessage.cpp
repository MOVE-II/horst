#include "ctlmessage.h"

#include <cstddef>
#include <iostream>
#include <sstream>

#include "../state/state.h"


namespace horst {

ControlMessage::ControlMessage() {}


std::unique_ptr<ControlMessage>
ControlMessage::parse(const std::string &msg) {

	std::unique_ptr<ControlMessage> ret = nullptr;

	std::cout << "[event] parsing control message: " << msg << std::endl;
	std::size_t first_space = msg.find(" ");

	if (first_space == std::string::npos) {
		std::cout << "[event] no parameters given to command" << std::endl;
		return ret;
	}

	std::string command_type = msg.substr(0, first_space);
	std::string rest         = msg.substr(first_space + 1);

	if (rest.size() >= 1 && rest.back() == '\n') {
		rest = rest.substr(0, rest.size() - 1);
	}

	if (rest.size() == 0) {
		std::cout << "[event] parameters have a length of 0" << std::endl;
		return ret;
	}

	if (command_type == "RUN") {
		ret = std::make_unique<ProcedureCallReq>(rest);
	}
	else if (command_type == "EXEC") {
		ret = std::make_unique<ShellCommandReq>(rest);
	}
	else if (command_type == "STATUS") {
		ret = std::make_unique<DaemonControlReq>(
			daemon_ctl_action_t::STATUS,
			rest
		);
	}
	else if (command_type == "START") {
		ret = std::make_unique<DaemonControlReq>(
			daemon_ctl_action_t::START,
			rest
		);
	}
	else if (command_type == "STOP") {
		ret = std::make_unique<DaemonControlReq>(
			daemon_ctl_action_t::STOP,
			rest
		);
	}
	else if (command_type == "RESTART") {
		ret = std::make_unique<DaemonControlReq>(
			daemon_ctl_action_t::RESTART,
			rest
		);
	}
	// TODO: more commands like REPORT => StatusReport of the satellite
	else {
		std::cout << "[event] unhandled command: " << command_type << std::endl;
	}

	return ret;
}


bool ControlMessage::is_fact() const {
	// control messages are never facts.
	// it's something the ground station requests.
	return false;
}


ProcedureCallReq::ProcedureCallReq(const std::string &name)
	:
	name{name} {}


void ProcedureCallReq::update(State &/*state*/) {
	// TODO: enqueue procedure
}


ShellCommandReq::ShellCommandReq(const std::string &command)
	:
	cmd{command} {}


void ShellCommandReq::update(State &state) {
	state.computer.shell_commands.push_back(this->cmd);
}


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

	state.computer.shell_commands.push_back(command.str());
}

}  // horst
