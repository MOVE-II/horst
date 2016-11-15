#include "daemon.h"

#include "error.h"


namespace horst {

Daemon::Daemon() {}

void Daemon::activate(const std::string &state_name) {
	auto it = this->states.find(state_name);

	if (it == std::end(this->states)) {
		throw Error{"could not find requested state"};
	}

	State *state = &it->second;

	state->activate(nullptr);
}


bool Daemon::is_active(const State *state) const {
	// TODO: dbus query for active state
}

} // horst
