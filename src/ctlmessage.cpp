#include "ctlmessage.h"

#include <cstddef>
#include <iostream>


namespace horst {

ControlMessage::ControlMessage(const std::string &msg) {
	std::cout << "create control message: " << msg << std::endl;
	std::size_t first_space = msg.find(" ");

	if (first_space == -1) {
		this->is_valid = false;
		return;
	}

	std::string command_type_str = msg.substr(0, first_space);
	std::string rest             = msg.substr(first_space + 1);

	if (rest.size() >= 1 && rest.back() == '\n') {
		rest = rest.substr(0, rest.size() - 1);
	}

	if (rest.size() == 0) {
		this->is_valid = false;
		return;
	}

	if (command_type_str.compare("RUN") == 0) {
		this->procedure_name = rest;
		this->type            = ctl_msg_t::RUN;
	}
	else if (command_type_str.compare("EXEC") == 0) {
		this->shell_command  = rest;
		this->type           = ctl_msg_t::EXEC;
	}
	else if (command_type_str.compare("STATE") == 0) {
		this->subsystem_id  = rest;
		this->type          = ctl_msg_t::STATE;
	}
	else if (command_type_str.compare("START") == 0) {
		this->subsystem_id  = rest;
		this->type          = ctl_msg_t::START;
	}
	else if (command_type_str.compare("STOP") == 0) {
		this->subsystem_id  = rest;
		this->type          = ctl_msg_t::STOP;
	}
	else if (command_type_str.compare("RESTART") == 0) {
		this->subsystem_id  = rest;
		this->type          = ctl_msg_t::RESTART;
	}
	else if (command_type_str.compare("STATUS") == 0) {
		this->subsystem_id  = rest;
		this->type          = ctl_msg_t::STATUS;
	}
	else {
		this->is_valid = false;
		return;
	}

	this->is_valid = true;
}

const std::string &ControlMessage::get_procedure_name() {
	return this->procedure_name;
}

const std::string &ControlMessage::get_subsystem_id() {
	return this->subsystem_id;
}

const std::string &ControlMessage::get_shell_command() {
	return this->shell_command;
}

bool ControlMessage::is_valid() {
	return this->is_valid;
}

ctl_msg_t ControlMessage::get_type() {
	return this->type;
}

}  // horst
