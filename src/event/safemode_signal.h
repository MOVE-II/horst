#pragma once

#include "event.h"


namespace horst {

/**
 * Emitted when the safemode was changed successfully.
 */
class SafeModeSignal : public Event {
public:
	SafeModeSignal(bool);
	virtual ~SafeModeSignal() = default;

	bool is_fact() const override;

	void update(State &state) override;

protected:
	bool safemode;
};

} // horst
