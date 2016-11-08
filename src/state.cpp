#include "state.h"

#include "daemon.h"

namespace horst {

State::State(Daemon *daemon,
             std::unordered_set<State *> requires,
             std::function<void()> to_activate)
	:
	daemon{daemon},
	requires{requires},
	to_activate{to_activate} {}


bool State::activate(State *for_state) {
	if (for_state != nullptr) {
		this->triggers.insert(for_state);
	}

	bool all_done = true;

	for (auto &state : this->requires) {
		if (not state->is_active()) {
			// TODO: this sends another activation
			//       when each required state of this state
			//       it activated
			//       -> add pending flag?
			state->activate(this);
			all_done = false;
		}
	}

	if (all_done) {
		this->to_activate();
		return true;
	}

	return false;
}


void State::on_activate() {
	std::unordered_set<State *> trigger_copy = this->triggers;

	for (auto &state : trigger_copy) {
		// trigger the activation of a state that depended on this one.
		bool was_activated = state->activate(this);

		if (was_activated) {
			this->triggers.erase(state);
		}
	}
}


bool State::is_active() const {
	return this->daemon->is_active(this);
}

} // horst
