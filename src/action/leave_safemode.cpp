#include "leave_safemode.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/safemode_signal.h"
#include "../logger.h"
#include "../satellite.h"


namespace horst {

LeaveSafeMode::LeaveSafeMode()
    : ShellCommand("leave_safemode.sh", nullptr) {}


std::string LeaveSafeMode::describe() const {
	std::stringstream ss;
	ss << "Leave safemode";
	return ss.str();
}


void LeaveSafeMode::perform(Satellite *sat, ac_done_cb_t done) {
	LOG_INFO("Leaving safemode");
	sat->on_event(std::make_shared<SafeModeSignal>(false));
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		LOG_INFO("Safemode was left");
		done(success, action);
	});
}

} // horst
