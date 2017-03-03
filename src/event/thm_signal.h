#pragma once

#include "../state/thm.h"
#include "event.h"


namespace horst {

/**
 * Emitted when the THM state changes
 */
class THMSignal : public Event {
public:
	THMSignal(THM::overall_temp temp);
	virtual ~THMSignal() = default;

	bool is_fact() const override;

	void update(State &state) override;
protected:
	THM::overall_temp temp;
};

} // horst
