#include "adcs_signal.h"

#include "../state/state.h"


namespace horst {

ADCSSignal::ADCSSignal(ADCS::adcs_state adcs_status) : adcs_status(adcs_status) {}

bool ADCSSignal::is_fact() const {
	return true;
}

void ADCSSignal::update(State &state) {
	state.adcs.pointing = this->adcs_status;
}

}  // horst
