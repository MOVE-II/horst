#pragma once

#include "shell_command.h"


namespace horst {

class LeaveSafeMode : public ShellCommand {
public:
	LeaveSafeMode();
	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;
};

} // horst
