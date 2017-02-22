#include "enter_manualmode.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/manualmode_signal.h"
#include "../satellite.h"


namespace horst {

EnterManualMode::EnterManualMode()
    : ShellCommand("./scripts/enter_manualmode.sh", nullptr) {}


std::string EnterManualMode::describe() const {
	std::stringstream ss;
	ss << "Enter manualmode";
	return ss.str();
}


void EnterManualMode::perform(Satellite *sat, ac_done_cb_t done) {
	std::cout << "Entering manualmode!" << std::endl;
	sat->on_event(std::make_shared<ManualModeSignal>(true));
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		if (success) {
			std::cout << "ManualMode was automatically left again" << std::endl;
			sat->on_event(std::make_shared<ManualModeSignal>(true));
		}
		done(success, action);
	});
}

} // horst
