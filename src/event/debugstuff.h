#pragma once

#include <functional>
#include <memory>
#include <string>

#include "event.h"


namespace horst {

/**
 * Emitted when the safe mode was entered successfully.
 */
class DebugStuff : public Event {
public:
	DebugStuff(const std::string &what_do);
	virtual ~DebugStuff() = default;

	bool is_fact() const override;

	void update(State &state) override;

protected:
	bool fact;
	std::string what_do;
};

} // horst
