#pragma once

#include <functional>
#include <string>


namespace horst {


class Action;
class Satellite;


/** callback type for notification when this action is done */
using ac_done_cb_t = std::function<void(bool success, Action *action)>;


/**
 * A thing that is invoked because horst needs to change something.
 * This may for example be "turn off battery and selfdestruct".
 */
class Action {
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
	 *
	 * The callback is called in the function
	 * when the action was performed.
	 */
	virtual void perform(Satellite *satellite,
	                     ac_done_cb_t done=nullptr) = 0;
};

} // horst
