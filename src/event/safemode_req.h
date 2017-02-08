#pragma once

#include <functional>
#include <memory>
#include <string>

#include "event.h"


namespace horst {

/**
 * Shows up if entering/leaving safe mode has been requested
 */
class SafeModeReq : public Event {
public:
	SafeModeReq(bool);
	virtual ~SafeModeReq() = default;

	bool is_fact() const override;

	void update(State &state) override;

protected:
	bool wanted;
};

} // horst
