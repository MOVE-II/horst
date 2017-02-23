#pragma once

#include <functional>
#include <memory>
#include <string>

#include "event.h"
#include "../state/payload.h"


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
