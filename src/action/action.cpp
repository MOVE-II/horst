#include "action.h"

#include "../satellite.h"


namespace horst {

Action::Action()
	:
	finished{nullptr} {}


void Action::call_when_done(done_cb_t callback) {
	this->finished = callback;
}

void Action::done() {
	if (this->finished != nullptr) {
		this->finished(this);
	}
}

} // horst
