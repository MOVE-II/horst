#include "enter_safemode.h"

#include <sstream>

#include "../event/maneuvermode_signal.h"
#include "../event/safemode_signal.h"
#include "../satellite.h"


namespace horst {

EnterSafeMode::EnterSafeMode()
    : ShellCommand("enter_safemode.sh", nullptr) {}


std::string EnterSafeMode::describe() const {
	std::stringstream ss;
	ss << "Enter safemode";
	return ss.str();
}


void EnterSafeMode::perform(Satellite *sat, ac_done_cb_t done) {
	LOG_INFO("[action] Entering safemode");

	// Always deactivate maneuvermode
	sat->on_event(std::make_shared<ManeuverModeSignal>(false));

	sat->on_event(std::make_shared<SafeModeSignal>(true));
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		LOG_INFO("[action] Safemode was entered");
		done(success, action);
	});
}

} // horst
