#pragma once

#include "event.h"


namespace horst {

/**
 * Emitted when the maneuvermode needs to be changed.
 */
class ManeuverModeSignal : public Event {
public:
	ManeuverModeSignal(bool);
	virtual ~ManeuverModeSignal() = default;

	bool is_fact() const override;

	void update(State &state) override;

protected:
	bool maneuvermode;
};

} // horst
