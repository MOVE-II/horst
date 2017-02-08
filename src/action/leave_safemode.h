#pragma once

#include <memory>

#include "action.h"

namespace horst {

class LeaveSafeMode : public Action {
public:
	LeaveSafeMode();

	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;
};

} // horst
