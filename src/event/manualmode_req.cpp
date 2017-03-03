#include "manualmode_req.h"

#include "../state/state.h"


namespace horst {

ManualModeReq::ManualModeReq(bool wanted) : wanted(wanted) {}

bool ManualModeReq::is_fact() const {
	return false;
}

void ManualModeReq::update(State &state) {
	state.manualmode = this->wanted;
}

}  // horst
