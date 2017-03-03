#include "thm_signal.h"

#include "../state/state.h"


namespace horst {

THMSignal::THMSignal(THM::overall_temp temp) : temp(temp) {}

bool THMSignal::is_fact() const {
	return true;
}

void THMSignal::update(State &state) {
	state.thm.all_temp = this->temp;
}

}  // horst
