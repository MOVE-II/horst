#pragma once

#include <memory>
#include <vector>

#include "../action/action.h"
#include "state_component.h"


namespace horst {


/**
 * Represents the state of the power supply.
 */
class EPS : public StateComponent {
public:
	EPS();

	std::vector<std::unique_ptr<Action>> transform_to(const EPS &target) const;

	/**
	 * battery level in [0.01]% (that means 1000 => 10% battery left)
	 */
	uint16_t battery_level;
};

} // horst
