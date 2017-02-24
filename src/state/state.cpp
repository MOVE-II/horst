#include "state.h"

#include "../action/enter_manualmode.h"
#include "../action/enter_safemode.h"
#include "../action/leave_manualmode.h"
#include "../action/leave_safemode.h"
#include "../action/trigger_sunpointing.h"
#include "../action/trigger_detumbling.h"
#include "../action/trigger_measuring.h"
#include "../logger.h"
#include "../util.h"

namespace horst {

/**
 * As soon as we reach an EPS battery level below that we will go into safemode
 */
static const uint16_t safemode_eps_treshold = 5000;

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
	     this->eps.battery_level < safemode_eps_treshold) and
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

	/**
	 * All following rules only apply if we are neither in safemode nor in
	 * manualmode
	 */
	if (!this->safemode && !this->manualmode) {

		if (target.pl.daemon == Payload::daemon_state::WANTMEASURE &&
		    this->eps.battery_level > safemode_eps_treshold &&
		    this->thm.all_temp == THM::overall_temp::OK &&
		    this->adcs.pointing == ADCS::adcs_state::SUN &&
		    (this->adcs.requested == ADCS::adcs_state::SUN ||
		    this->adcs.requested == ADCS::adcs_state::NONE) &&
		    this->pl.daemon != Payload::daemon_state::MEASURING &&
		    this->leop == leop_seq::DONE) {
			/* Start measuring */
			ret.push_back(std::make_unique<TriggerMeasuring>());
		}

		if (this->adcs.pointing != ADCS::adcs_state::SUN &&
		    this->adcs.pointing != ADCS::adcs_state::DETUMB &&
		    this->adcs.requested != ADCS::adcs_state::SUN &&
		    this->adcs.requested != ADCS::adcs_state::DETUMB &&
		    this->leop != leop_seq::UNDEPLOYED) {
			/* Start detumbling */
			ret.push_back(std::make_unique<TriggerDetumbling>());
		}

		if (this->adcs.pointing == ADCS::adcs_state::DETUMB &&
		    this->leop != leop_seq::UNDEPLOYED) {
			/* After detumbling always trigger sunpointing */
			ret.push_back(std::make_unique<TriggerSunpointing>());
		}

		if (this->eps.battery_level > safemode_eps_treshold &&
		    this->thm.all_temp == THM::overall_temp::OK &&
		    this->adcs.pointing == ADCS::adcs_state::SUN &&
		    this->pl.daemon == Payload::daemon_state::WANTMEASURE &&
		    this->leop == leop_seq::DONE) {
			/* Start measuring */
			ret.push_back(std::make_unique<TriggerMeasuring>());
		}
	}

	return ret;
}

State::leop_seq State::str2leop(const char* name) {
	switch(util::str2int(name)) {
	case util::str2int("UNDEPLOYED"):
		return State::leop_seq::UNDEPLOYED;
	case util::str2int("DEPLOYED"):
		return State::leop_seq::DEPLOYED;
	case util::str2int("DONE"):
		return State::leop_seq::DONE;
	default:
		LOG_WARN("Could not interpret '" + std::string(name) + "' as leop sequence!");
		return State::leop_seq::UNDEPLOYED;
	}
}

} // horst
