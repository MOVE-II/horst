#include "leave_safemode.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/safemode_signal.h"
#include "../satellite.h"


namespace horst {

LeaveSafeMode::LeaveSafeMode()
    : ShellCommand("./scripts/leave_safemode.sh", nullptr) {}


std::string LeaveSafeMode::describe() const {
	std::stringstream ss;
	ss << "Leave safemode";
	return ss.str();
}


void LeaveSafeMode::perform(Satellite *sat, ac_done_cb_t done) {
	std::cout << "Start leaving safemode!" << std::endl;
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		std::cout << "Safemode was left." << std::endl;
		if (success) {
			sat->on_event(std::make_shared<SafeModeSignal>(false));
		} else {
			// TODO: We should do sth. here. Maybe resetting
			// requested state, if we have one
		}
		done(success, action);
	});
}

} // horst
