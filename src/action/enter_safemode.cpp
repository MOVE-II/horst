#include "enter_safemode.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/safemode_signal.h"
#include "../satellite.h"


namespace horst {

EnterSafeMode::EnterSafeMode() {}


std::string EnterSafeMode::describe() const {
	std::stringstream ss;
	ss << "enter safe mode";
	return ss.str();
}


void EnterSafeMode::perform(Satellite *sat, ac_done_cb_t done) {
	std::cout << "safe mode was entered!" << std::endl;
	sat->on_event(std::make_shared<SafeModeSignal>(true));

	done(true, this);
}

} // horst
