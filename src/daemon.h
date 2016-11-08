#pragma once

#include <unordered_map>

#include "state.h"

namespace horst {

class Daemon {
public:
	Daemon();
	virtual ~Daemon() = default;

	void activate(const std::string &state_name);
	bool is_active(const State *state) const;

private:
	std::unordered_map<std::string, State> states;
	State *current_state;
};

} // horst
