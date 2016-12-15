#include "shell_command.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../process.h"
#include "../satellite.h"


namespace horst {

ShellCommand::ShellCommand(const std::string &cmd)
	:
	process{nullptr},
	cmd{cmd} {}


std::string ShellCommand::describe() const {
	std::stringstream ss;
	ss << "execute shell command: '" << this->cmd << "'";
	return ss.str();
}


void ShellCommand::perform(Satellite *sat) {
	this->process = std::make_unique<Process>(
		sat->get_loop(), this->cmd,
		[this] (Process *, int64_t exit_code) {
			std::cout << "[process] exited with " << exit_code << std::endl;
			this->done();
		}
	);
}

} // horst
