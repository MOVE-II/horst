#pragma once

namespace horst {

class State;


/**
 * An external event that is handled by horst.
 */
class Event {
public:
	Event();
	virtual ~Event() = default;

	/**
	 * Return true if this event describes a fact.
	 * A fact is some naturally given property like "battery empty".
	 */
	virtual bool is_fact() const = 0;

	/**
	 * Update the given satellite state with this event.
	 */
	virtual void update(State &state) = 0;
};

} // horst
