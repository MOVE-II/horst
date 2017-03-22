#pragma once

#include "event.h"
#include "../state/state.h"


namespace horst {

/**
 * Shows up if leop state change has been requested
 */
class LEOPReq : public Event {
public:
	LEOPReq(State::leop_seq);
	virtual ~LEOPReq() = default;

	bool is_fact() const override;

	void update(State &state) override;

protected:
	State::leop_seq wanted;
};

} // horst
