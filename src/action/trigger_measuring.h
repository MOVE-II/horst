#pragma once

#include "shell_command.h"


namespace horst {

class TriggerMeasuring : public ShellCommand {
public:
	TriggerMeasuring();
	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;
};

} // horst
