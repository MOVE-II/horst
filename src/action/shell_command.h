#pragma once

#include "action.h"

namespace horst {

class ShellCommand : public Action {
public:
	ShellCommand(const std::string &cmd);

	std::string describe() const override;
	void perform(Satellite *satellite) override;

protected:
	std::string cmd;
};

} // horst
