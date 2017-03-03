#pragma once

#include "../state/payload.h"
#include "event.h"


namespace horst {

/**
 * Emitted when the payload state changed
 */
class PayloadSignal : public Event {
public:
	PayloadSignal(Payload::daemon_state);
	virtual ~PayloadSignal() = default;

	bool is_fact() const override;

	void update(State &state) override;

protected:
	Payload::daemon_state payload;
};

} // horst
