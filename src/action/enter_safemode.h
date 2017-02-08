#pragma once

#include <memory>

#include "action.h"

namespace horst {

class EnterSafeMode : public Action {
public:
	EnterSafeMode();

	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;
};

} // horst
