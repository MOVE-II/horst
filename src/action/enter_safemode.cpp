#include "enter_safemode.h"

#include <sstream>

#include "../event/maneuvermode_signal.h"
#include "../event/safemode_signal.h"
#include "../satellite.h"


namespace horst {

EnterSafeMode::EnterSafeMode(uint8_t reason)
    : ShellCommand("enter_safemode.sh", nullptr), reason(reason) {}


std::string EnterSafeMode::describe() const {
	std::stringstream ss;
	ss << "Enter safemode";
	return ss.str();
}


void EnterSafeMode::perform(Satellite *sat, ac_done_cb_t done) {
	LOG_INFO("[action] Entering safemode");

	// Set safemode to true first, to not reenter it multiple times in a
	// loop
	sat->on_event(std::make_shared<SafeModeSignal>(this->reason));

	// Always deactivate maneuvermode
	sat->on_event(std::make_shared<ManeuverModeSignal>(false));

	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		if (success) {
			LOG_INFO("[action] Safemode was entered");
		} else {
			LOG_WARN("[action] Failed to enter safemode!");
			sat->on_event(std::make_shared<SafeModeSignal>(0));
		}
		done(success, action);
	});
}

} // horst
