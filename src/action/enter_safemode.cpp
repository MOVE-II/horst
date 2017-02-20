#include "enter_safemode.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/safemode_signal.h"
#include "../satellite.h"


namespace horst {

EnterSafeMode::EnterSafeMode()
    : ShellCommand("./scripts/enter_safemode.sh", nullptr) {}


std::string EnterSafeMode::describe() const {
	std::stringstream ss;
	ss << "Enter safe mode";
	return ss.str();
}


void EnterSafeMode::perform(Satellite *sat, ac_done_cb_t done) {
	std::cout << "Start entering safe mode!" << std::endl;
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		std::cout << "Safe mode was entered!" << std::endl;
		sat->on_event(std::make_shared<SafeModeSignal>(true));
		done(success, action);
	});
}

} // horst
