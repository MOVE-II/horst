#include "shell_command.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../process.h"
#include "../satellite.h"


namespace horst {

ShellCommand::ShellCommand(const std::string &cmd)
	:
	cmd{cmd} {}


std::string ShellCommand::describe() const {
	std::stringstream ss;
	ss << "execute shell command: '" << this->cmd << "'";
	return ss.str();
}


void ShellCommand::perform(Satellite *sat) {
	auto process = std::make_unique<Process>(sat->get_loop(), this->cmd);

	sat->add_process(std::move(process));
}

} // horst
