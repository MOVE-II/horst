#pragma once

#include <memory>
#include <vector>

#include "../action/action.h"
#include "state_component.h"


namespace horst {


/**
 * Represents the state of the power supply.
 */
class COM : public StateComponent {
public:
	COM();

	std::vector<std::unique_ptr<Action>> transform_to(const COM &target);

	/** temperature state of sband */
	enum class sband_active {
		YES,
		NO
	} sband_active;
};

} // horst
