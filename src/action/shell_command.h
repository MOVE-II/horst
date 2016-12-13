#pragma once

#include <memory>

#include "action.h"

namespace horst {

class Process;


class ShellCommand : public Action {
public:
	ShellCommand(const std::string &cmd);

	std::string describe() const override;
	void perform(Satellite *satellite) override;

protected:
	std::unique_ptr<Process> process;
	std::string cmd;
};

} // horst
