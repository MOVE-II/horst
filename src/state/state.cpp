#include "state.h"

#include "../util.h"
#include "../action/enter_safe_mode.h"


namespace horst {

State::State()
	:
	safe_mode{false} {}

State State::copy() const {
	return State{*this};
}


std::vector<std::unique_ptr<Action>> State::transform_to(const State &target) const {
	// this function is the "state transition table"
	// it calculates what actions are required to reach the target state.

	std::vector<std::unique_ptr<Action>> ret;

	// perform the computer state transition
	util::vector_extend(ret, this->computer.transform_to(target.computer));


	if ((this->thm.all_temp == THM::all_temp::ALARM or
	     this->eps.battery_level < 20) and
	    this->safe_mode == false) {

		ret.push_back(std::make_unique<EnterSafeMode>());
	}

	return ret;
}

} // horst
