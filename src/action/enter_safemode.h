#pragma once

#include "shell_command.h"


namespace horst {

class EnterSafeMode : public ShellCommand {
public:
	EnterSafeMode(uint8_t);
	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;
private:
	uint8_t reason;
};

} // horst
