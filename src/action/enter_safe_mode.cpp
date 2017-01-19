#include "enter_safe_mode.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/safe_mode_entered.h"
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
	sat->on_event(std::make_shared<SafeModeEntered>());

	done(true, this);
}

} // horst
