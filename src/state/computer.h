#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "../action/action.h"
#include "state_component.h"


namespace horst {

class ControlMessage;


/**
 * Represents the state of this board computer.
 */
class Computer : public StateComponent {
public:
	Computer();

	std::vector<std::unique_ptr<Action>> transform_to(const Computer &target) const;

	/** list of shell commands to fire. */
	std::vector<
		std::tuple<
			std::string,
			std::shared_ptr<ControlMessage>>> shell_commands;

	/** list of procedures to call */
	std::vector<
		std::tuple<
			std::string,
			std::shared_ptr<ControlMessage>>> procedures;
};

} // horst
