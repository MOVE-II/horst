#include "leave_manualmode.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/manualmode_signal.h"
#include "../satellite.h"


namespace horst {

LeaveManualMode::LeaveManualMode()
    : ShellCommand("./scripts/leave_manualmode.sh", nullptr) {}


std::string LeaveManualMode::describe() const {
	std::stringstream ss;
	ss << "Leave manualmode";
	return ss.str();
}


void LeaveManualMode::perform(Satellite *sat, ac_done_cb_t done) {
	std::cout << "Start leaving manualmode!" << std::endl;
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		std::cout << "Manualmode was left." << std::endl;
		if (success) {
			sat->on_event(std::make_shared<ManualModeSignal>(false));
		} else {
			// TODO: We should do sth. here. Maybe resetting
			// requested state, if we have one
		}
		done(success, action);
	});
}

} // horst
