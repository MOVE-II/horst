#pragma once

#include "event.h"


namespace horst {

/**
 * Emitted when the EPS battery state changes
 */
class EPSSignal : public Event {
public:
	EPSSignal(uint16_t);
	virtual ~EPSSignal() = default;

	bool is_fact() const override;

	void update(State &state) override;
protected:
	uint16_t bat;
};

} // horst
