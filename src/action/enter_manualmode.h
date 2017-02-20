#pragma once

#include "shell_command.h"

namespace horst {

class EnterManualMode : public ShellCommand {
public:
	EnterManualMode();
	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;
};

} // horst
