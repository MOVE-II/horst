#pragma once

#include <functional>
#include <string>


namespace horst {

class Satellite;


/**
 * A thing that is invoked because horst needs to change something.
 * This may for example be "turn off battery and selfdestruct".
 */
class Action {
	/** callback type for notification when this action is done */
	using done_cb_t = std::function<void(Action *action)>;

public:
	Action();
	virtual ~Action() = default;

	/**
	 * A textual description of what the action is doing.
	 */
	virtual std::string describe() const = 0;

	/**
	 * Perform the action.
	 * This may mean to enqueue something in the event loop,
	 * or whatever is needed to do.
	 * Override it for each action type.
	 */
	virtual void perform(Satellite *satellite) = 0;

	/**
	 * Register a callback.
	 */
	void call_when_done(done_cb_t callback);

protected:

	/** call this when the action is finished */
	void done();

	/**
	 * Called by done().
	 * Might be a list of callbacks someday if we need it.
	 */
	done_cb_t finished;
};

} // horst
