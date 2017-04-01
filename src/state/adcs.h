#pragma once

#include <memory>
#include <vector>

#include "../action/action.h"
#include "state_component.h"


namespace horst {


/**
 * Represents the state of the power supply.
 */
class ADCS : public StateComponent {
public:
	ADCS();

	std::vector<std::unique_ptr<Action>> transform_to(const ADCS &target) const;

	/**
	 * Available adcs modes
	 */
	enum class adcs_state {
		NONE,
		SLEEP,
		ATTDET,
		DETUMB,
		NADIR,
		SUN,
		FLASH,
		EXP
	};

	/**
	 * Where are we pointing to currently
	 */
	enum adcs_state pointing;

	/**
	 * What pointing we did request
	 */
	enum adcs_state requested;

	static ADCS::adcs_state str2state(char* name);
};

} // horst
