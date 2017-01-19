#include "safe_mode_entered.h"

#include <cstddef>
#include <iostream>

#include "../state/state.h"


namespace horst {

SafeModeEntered::SafeModeEntered() {}

bool SafeModeEntered::is_fact() const {
	return true;
}

void SafeModeEntered::update(State &state) {
	state.safe_mode = true;
}

}  // horst
