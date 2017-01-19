#pragma once

#include <functional>
#include <memory>
#include <string>

#include "event.h"


namespace horst {

/**
 * Emitted when the safe mode was entered successfully.
 */
class SafeModeEntered : public Event {
public:
	SafeModeEntered();
	virtual ~SafeModeEntered() = default;

	bool is_fact() const override;

	void update(State &state) override;
};

} // horst
