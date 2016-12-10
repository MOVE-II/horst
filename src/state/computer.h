#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../action/action.h"
#include "state_component.h"


namespace horst {

/**
 * Represents the state of this board computer.
 */
class Computer : public StateComponent {
public:
	Computer();

	std::vector<std::string> shell_commands;

	std::vector<std::unique_ptr<Action>> transform_to(const Computer &target);
};

} // horst
