#include "maneuvermode_signal.h"

#include <cstddef>
#include <iostream>

#include "../state/state.h"


namespace horst {

ManeuverModeSignal::ManeuverModeSignal(bool maneuvermode) : maneuvermode(maneuvermode) {}

bool ManeuverModeSignal::is_fact() const {
	return true;
}

void ManeuverModeSignal::update(State &state) {
	state.maneuvermode = this->maneuvermode;
}

}  // horst
