#include "req_shell_command.h"

#include <tuple>

#include "../state/state.h"


namespace horst {

ShellCommandReq::ShellCommandReq(const std::string &command)
	:
	cmd{command} {}


void ShellCommandReq::update(State &state) {
	// to reach the target state, just add the shell command
	// to the execution list.

	state.computer.shell_commands.push_back(
		std::make_tuple(
			this->cmd,
			std::static_pointer_cast<ControlMessage>(this->shared_from_this())
		)
	);
}

} // horst
