#include "leave_manualmode.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/manualmode_signal.h"
#include "../satellite.h"


namespace horst {

LeaveManualMode::LeaveManualMode()
    : ShellCommand("leave_manualmode.sh", nullptr) {}


std::string LeaveManualMode::describe() const {
	std::stringstream ss;
	ss << "Leave manualmode";
	return ss.str();
}


void LeaveManualMode::perform(Satellite *sat, ac_done_cb_t done) {
	LOG_INFO("[action] Leaving manualmode");
	sat->on_event(std::make_shared<ManualModeSignal>(false));
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		LOG_INFO("[action] Manualmode was left");
		done(success, action);
	});
}

} // horst
