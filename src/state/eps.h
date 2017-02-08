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
	 * Power state of sband.
	 * The WAIT_* states are used to store that the switch
	 * was requested but not yet confirmed.
	 */
	enum class sband_state {
		OFF,
		WAIT_OFF,
		ON,
		WAIT_ON
	} sband;

	/**
	 * battery level in [0.01]% (that means 1000 => 10% battery left)
	 */
	uint16_t battery_level;
};

} // horst
