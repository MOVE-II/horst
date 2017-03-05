#pragma once

#include "../state/state.h"
#include "event.h"


namespace horst {

/**
 * Emitted when the LEOP state changes
 */
class LEOPSignal : public Event {
public:
	LEOPSignal(State::leop_seq leop);
	virtual ~LEOPSignal() = default;

	bool is_fact() const override;

	void update(State &state) override;
protected:
	State::leop_seq leop;
};

} // horst
