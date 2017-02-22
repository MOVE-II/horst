#include "state.h"

#include "../action/enter_manualmode.h"
#include "../action/enter_safemode.h"
#include "../action/leave_manualmode.h"
#include "../action/leave_safemode.h"
#include "../util.h"


namespace horst {

State::State()
	:
	safemode{false}, manualmode{false} {}

State State::copy() const {
	return State{*this};
}


std::vector<std::unique_ptr<Action>> State::transform_to(const State &target) const {
	// this function is the "state transition table"
	// it calculates what actions are required to reach the target state.

	std::vector<std::unique_ptr<Action>> ret;

	// perform the computer state transition
	util::vector_extend(ret, this->computer.transform_to(target.computer));

	/* Handle requests for entering/leaving manualmode */
	if (target.manualmode && !this->manualmode) {
		/* Go to manualmode */
		ret.push_back(std::make_unique<EnterManualMode>());
	} else if (!target.manualmode && this->manualmode) {
		/* Leave manualmode */
		ret.push_back(std::make_unique<LeaveManualMode>());
	}

	/* Enter safemode in emergency case */
	if ((this->thm.all_temp == THM::overall_temp::ALARM or
	     this->eps.battery_level < 2000) and
	    this->safemode == false) {
		ret.push_back(std::make_unique<EnterSafeMode>());
	}

	/* Handle requests for entering/leaving safemode */
	if (target.safemode && !this->safemode) {
		/* Go to safemode */
		ret.push_back(std::make_unique<EnterSafeMode>());
	} else if (!target.safemode && this->safemode) {
		/* Leave safemode */
		ret.push_back(std::make_unique<LeaveSafeMode>());
	}


	return ret;
}

} // horst
