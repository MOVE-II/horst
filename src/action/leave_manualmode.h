#pragma once

#include "shell_command.h"

namespace horst {

class LeaveManualMode : public ShellCommand {
public:
	LeaveManualMode();
	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;
};

} // horst
