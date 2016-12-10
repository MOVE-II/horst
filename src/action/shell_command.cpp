#include "shell_command.h"

#include <iostream>
#include <sstream>


namespace horst {

ShellCommand::ShellCommand(const std::string &cmd)
	:
	cmd{cmd} {}


std::string ShellCommand::describe() const {
	std::stringstream ss;
	ss << "execute shell command: '" << this->cmd << "'";
	return ss.str();
}


void ShellCommand::perform(Satellite *) {
	std::cout << "TODO execute: " << this->cmd << std::endl;
}

} // horst
