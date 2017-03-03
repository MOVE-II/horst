#include "payload_signal.h"

#include "../state/state.h"


namespace horst {

PayloadSignal::PayloadSignal(Payload::daemon_state payload) : payload(payload) {}

bool PayloadSignal::is_fact() const {
	return true;
}

void PayloadSignal::update(State &state) {
	state.pl.daemon = this->payload;
}

} // horst
