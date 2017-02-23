#include "payload_req.h"

#include <cstddef>
#include <iostream>

#include "../state/state.h"

namespace horst {

PayloadReq::PayloadReq(Payload::daemon_state wanted) : wanted(wanted) {}

bool PayloadReq::is_fact() const {
	return false;
}

void PayloadReq::update(State &state) {
	state.pl.daemon = this->wanted;
}

}  // horst
