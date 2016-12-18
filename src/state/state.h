#pragma once


#include <memory>
#include <vector>

#include "../action/action.h"
#include "computer.h"


namespace horst {

/**
 * State of the satellite.
 * Events update this state.
 */
class State {
public:
	State();
	virtual ~State() = default;

	/**
	 * Create a copy of this state.
	 * Use this to create the new target state.
	 */
	State copy() const;

	/**
	 * Return a list of actions necessary to perform
	 * to reach the given target state of the satellite.
	 */
	std::vector<std::unique_ptr<Action>>
	transform_to(const State &target);

	// TODO: list of properties that make up the state.
	//       the properties are attached to subsystem daemons.

	/**
	 * State of the satellite's main computer.
	 *
	 * Used for changes required on the system like program starts,
	 * shell commands etc.
	 */
	Computer computer;
};

} // horst
