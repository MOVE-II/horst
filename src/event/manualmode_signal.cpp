#include "manualmode_signal.h"

#include <cstddef>
#include <iostream>

#include "../state/state.h"


namespace horst {

ManualModeSignal::ManualModeSignal(bool manualmode) : manualmode(manualmode) {}

bool ManualModeSignal::is_fact() const {
	return true;
}

void ManualModeSignal::update(State &state) {
	state.manualmode = this->manualmode;
}

} // horst
