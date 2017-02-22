#pragma once

#include <functional>
#include <memory>
#include <string>

#include "event.h"


namespace horst {

/**
 * Emitted when the manualmode was changed successfully.
 */
class ManualModeSignal : public Event {
public:
	ManualModeSignal(bool);
	virtual ~ManualModeSignal() = default;

	bool is_fact() const override;

	void update(State &state) override;

protected:
	bool manualmode;
};

} // horst
