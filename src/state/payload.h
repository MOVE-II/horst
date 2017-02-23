#pragma once

#include <memory>
#include <vector>

#include "../action/action.h"
#include "state_component.h"


namespace horst {


/**
 * Represents the state of the power supply.
 */
class Payload : public StateComponent {
public:
	Payload();

	std::vector<std::unique_ptr<Action>> transform_to(const Payload &target) const;

	/**
	 * Daemon status
	 */
	enum class daemon_state {
		OFF,
		IDLE,
		WANTMEASURE,
		MEASURING
	} daemon;
};

} // horst
