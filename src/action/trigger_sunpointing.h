#pragma once

#include "shell_command.h"

namespace horst {

class TriggerSunpointing : public ShellCommand {
public:
	TriggerSunpointing();
	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;
};

} // horst
