#pragma once

#include <memory>
#include <vector>

#include "../action/action.h"
#include "state_component.h"


namespace horst {


/**
 * Represents the state of the power supply.
 */
class THM : public StateComponent {
public:
	THM();

	std::vector<std::unique_ptr<Action>> transform_to(const THM &target);

	/**
	 * Temperature state of sband.
	 * It is a natural fact and can't be changed directly.
	 */
	enum class sband_temp {
		GOOD,
		BAD
	} sband_temp;

	/**
	 * overall temperature state of the satellite.
	 */
	enum class all_temp {
		OK,
		WARN,
		ALARM,
	} all_temp;
};

} // horst
