#pragma once


#include <memory>
#include <vector>

#include "../action/action.h"
#include "com.h"
#include "computer.h"
#include "eps.h"
#include "thm.h"


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
	 *
	 * This function is the most important one of the whole satellite.
	 * All the logic of when to do what is implemented here!!!!
	 */
	std::vector<std::unique_ptr<Action>>
	transform_to(const State &target) const;

	/**
	 * State of the satellite's main computer.
	 *
	 * Used for changes required on the system like program starts,
	 * shell commands etc.
	 */
	Computer computer;

	/** safe mode is active on the satellite */
	bool safemode;

	/** State of the power supply */
	EPS eps;

	/** State of the COM transciever */
	COM com;

	/** State of the thermal subsystem */
	THM thm;
};

} // horst
