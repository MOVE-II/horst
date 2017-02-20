#include "leave_safemode.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/safemode_signal.h"
#include "../satellite.h"


namespace horst {

LeaveSafeMode::LeaveSafeMode() {}


std::string LeaveSafeMode::describe() const {
	std::stringstream ss;
	ss << "enter safe mode";
	return ss.str();
}


void LeaveSafeMode::perform(Satellite *sat, ac_done_cb_t done) {
	std::cout << "safe mode was left!" << std::endl;
	sat->on_event(std::make_shared<SafeModeSignal>(false));

	done(true, this);
}

} // horst
