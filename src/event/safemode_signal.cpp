#include "safemode_signal.h"

#include <cstddef>
#include <iostream>

#include "../state/state.h"


namespace horst {

SafeModeSignal::SafeModeSignal(bool safemode) : safemode(safemode) {}

bool SafeModeSignal::is_fact() const {
	return true;
}

void SafeModeSignal::update(State &state) {
	state.safemode = this->safemode;
}

}  // horst
