#include "computer.h"

#include "../action/shell_command.h"
#include "../process.h"


namespace horst {

Computer::Computer() {}

std::vector<std::unique_ptr<Action>>
Computer::transform_to(const Computer &target) const {

	std::vector<std::unique_ptr<Action>> ret;

	// stupidly add all shell executions
	// in the future, this could be done more intelligent
	// to select only some of the commands, but fuck it for now.
	// this also provides the originating ControlMessage.
	for (auto &cmd_request : target.shell_commands) {
		ret.push_back(
			std::make_unique<ShellCommand>(
				std::get<0>(cmd_request),  // the command
				"",
				std::get<1>(cmd_request)   // the request
			)
		);
	}

	return ret;
}

} // horst
