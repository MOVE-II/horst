#include "leop_req.h"

namespace horst {

LEOPReq::LEOPReq(State::leop_seq wanted) : wanted(wanted) {}

bool LEOPReq::is_fact() const {
	return false;
}

void LEOPReq::update(State &state) {
	state.leop = this->wanted;
}

}  // horst
