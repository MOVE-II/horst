#pragma once


#include <functional>
#include <unordered_set>

namespace horst {

class Daemon;


class State {
public:
	State(Daemon *daemon,
	      std::unordered_set<State *> requires,
	      std::function<void()> to_activate);

	virtual ~State() = default;

	/**
	 * Request the activation of this state.
	 * Some state requested this, which is passed as argument.
	 *
	 * @returns if the actual activation function (to_activate) was called,
	 *          it is not called when other states are not active yet.
	 */
	bool activate(State *for_state);

	/**
	 * Called when this state actually became active.
	 */
	void on_activate();

	/**
	 * Called when this state actually became inactive.
	 */
	void on_deactivate();

	/**
	 * Check if this state is active.
	 */
	bool is_active() const;

protected:
	/**
	 * The subsystem daemon this state is assigned to.
	 */
	Daemon *daemon;

	/**
	 * The states this state depends on.
	 */
	std::unordered_set<State *> requires;

	/**
	 * A function that is executed to activate this state.
	 * The normal case will be the transmission of a dbus message.
	 */
	std::function<void()> to_activate;

	/**
	 * Function that is executed to deactivate this state.
	 */
	std::function<void()> to_deactivate;

private:

	/**
	 * Set of states that are notified once this state became active.
	 */
	std::unordered_set<State *> triggers;
};

} // horst
