#include "enter_manualmode.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/manualmode_signal.h"
#include "../logger.h"
#include "../satellite.h"


namespace horst {

EnterManualMode::EnterManualMode()
    : ShellCommand("enter_manualmode.sh", nullptr) {}


std::string EnterManualMode::describe() const {
	std::stringstream ss;
	ss << "Enter manualmode";
	return ss.str();
}


void EnterManualMode::perform(Satellite *sat, ac_done_cb_t done) {
	LOG_INFO("[action] Entering manual mode");
	sat->on_event(std::make_shared<ManualModeSignal>(true));
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		if (success) {
			LOG_INFO("[action] Manualmode was automatically left again");
		}
		done(success, action);
	});
}

} // horst
