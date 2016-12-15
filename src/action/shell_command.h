#pragma once

#include <memory>

#include "action.h"

namespace horst {

class Process;
class ControlMessage;


class ShellCommand : public Action {
public:
	ShellCommand(const std::string &cmd, std::shared_ptr<ControlMessage> request=nullptr);

	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;

protected:
	/** the running process */
	std::unique_ptr<Process> process;

	/** command to execute */
	std::string command;

	/** the request that lead to the execution of this shellcommand */
	std::shared_ptr<ControlMessage> request;
};

} // horst
