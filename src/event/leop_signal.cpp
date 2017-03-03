#include "leop_signal.h"


namespace horst {

LEOPSignal::LEOPSignal(State::leop_seq leop) : leop(leop) {}

bool LEOPSignal::is_fact() const {
	return true;
}

void LEOPSignal::update(State &state) {
	state.leop = this->leop;
}

}  // horst
