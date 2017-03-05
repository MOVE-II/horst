#pragma once

#include "event.h"


namespace horst {

/**
 * Shows up if entering/leaving manualmode has been requested
 */
class ManualModeReq : public Event {
public:
	ManualModeReq(bool);
	virtual ~ManualModeReq() = default;

	bool is_fact() const override;

	void update(State &state) override;

protected:
	bool wanted;
};

} // horst
