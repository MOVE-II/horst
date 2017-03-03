#include "eps_signal.h"

#include "../state/state.h"


namespace horst {

EPSSignal::EPSSignal(uint16_t bat) : bat(bat) {}

bool EPSSignal::is_fact() const {
	return true;
}

void EPSSignal::update(State &state) {
	state.eps.battery_level = this->bat;
}

}  // horst
