#pragma once

#include "shell_command.h"

namespace horst {

class TriggerDetumbling : public ShellCommand {
public:
	TriggerDetumbling();
	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;
};

} // horst
