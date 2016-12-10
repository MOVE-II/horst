#include "action.h"

#include "../satellite.h"


namespace horst {

Action::Action() {}

std::string Action::describe() const {
	return "noop default action";
}


void Action::perform(Satellite *) {
	// do nothing.
	return;
}

} // horst
