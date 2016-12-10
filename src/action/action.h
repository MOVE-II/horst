#pragma once

#include <string>


namespace horst {

class Satellite;


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
	virtual std::string describe() const;

	/**
	 * Perform the action.
	 * This may mean to enqueue something in the event loop,
	 * or whatever is needed to do.
	 * Override it for each action type.
	 */
	virtual void perform(Satellite *satellite);
};

} // horst
