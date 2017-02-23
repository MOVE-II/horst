#pragma once

#include <functional>
#include <memory>
#include <string>

#include "../state/adcs.h"
#include "event.h"


namespace horst {

/**
 * Emitted when the ADCS state changes
 */
class ADCSSignal : public Event {
public:
	ADCSSignal(ADCS::adcs_state adcs_status);
	virtual ~ADCSSignal() = default;

	bool is_fact() const override;

	void update(State &state) override;
protected:
	ADCS::adcs_state adcs_status;
};

} // horst
