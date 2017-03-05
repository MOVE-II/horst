#pragma once

#include "../state/payload.h"
#include "event.h"


namespace horst {

/**
 * Shows up if entering/leaving manualmode has been requested
 */
class PayloadReq : public Event {
public:
	PayloadReq(Payload::daemon_state);
	virtual ~PayloadReq() = default;

	bool is_fact() const override;

	void update(State &state) override;

protected:
	Payload::daemon_state wanted;
};

} // horst
