#include "computer.h"

#include "../action/shell_command.h"

#include "../process.h"


namespace horst {

Computer::Computer() {}

std::vector<std::unique_ptr<Action>>
Computer::transform_to(const Computer &target) {

	std::vector<std::unique_ptr<Action>> ret;

	// stupidly add all shell executions
	// in the future, this could be done more intelligent
	// to select only some of the commands, but fuck it for now.
	for (auto &cmd : target.shell_commands) {
		ret.push_back(std::make_unique<ShellCommand>(cmd));
	}

	return ret;
}

} // horst
