#include "trigger_detumbling.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../logger.h"
#include "../satellite.h"
#include "../state/adcs.h"
#include "../event/adcs_signal.h"
#include "../event/adcs_req_signal.h"


namespace horst {

TriggerDetumbling::TriggerDetumbling()
    : ShellCommand("trigger_detumbling.sh", nullptr) {}


std::string TriggerDetumbling::describe() const {
	std::stringstream ss;
	ss << "Trigger detumbling on adcs daemon";
	return ss.str();
}


void TriggerDetumbling::perform(Satellite *sat, ac_done_cb_t done) {
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		if (success) {
			LOG_INFO("[action] Detumbling has been triggered");
			// Create adcs state change signal for detumbling requested
			sat->on_event(std::make_shared<ADCSreqSignal>(ADCS::adcs_state::DETUMB));
		} else {
			// reschedule a retry ?
		}
		done(success, action);
	});
}

} // horst
