#pragma once


#include <functional>
#include <unordered_set>

namespace horst {

class Daemon;


class State {
public:
	State(Daemon *daemon);

	virtual ~State() = default;

protected:
	/**
	 * The subsystem daemon this state is assigned to.
	 */
	Daemon *daemon;
};

} // horst
