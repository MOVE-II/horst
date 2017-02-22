#include "debugstuff.h"

#include <cstddef>
#include <iostream>

#include "../logger.h"
#include "../state/state.h"


namespace horst {

DebugStuff::DebugStuff(const std::string &what_do)
	:
	fact{true},
	what_do{what_do} {}

bool DebugStuff::is_fact() const {
	// if you want this to be a request,
	// check here for the this->what_do and return an appropriate
	// value.
	return this->fact;
}

void DebugStuff::update(State &state) {

	if (this->what_do == "bat") {
		LOG_INFO("Forcing battery level");
		state.eps.battery_level = 15;
	} else {
		LOG_INFO("Unknown debug command: " + std::string(this->what_do));
	}
}

}  // horst
