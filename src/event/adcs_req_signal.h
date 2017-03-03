#pragma once

#include "../state/adcs.h"
#include "event.h"


namespace horst {

/**
 * Emitted when the ADCS state changes
 */
class ADCSreqSignal : public Event {
public:
	ADCSreqSignal(ADCS::adcs_state adcs_status);
	virtual ~ADCSreqSignal() = default;

	bool is_fact() const override;

	void update(State &state) override;
protected:
	ADCS::adcs_state adcs_status;
};

} // horst
