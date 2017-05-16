#pragma once

#include <memory>

#include "action.h"
#include "../process.h"

namespace horst {

class ControlMessage;


class ShellCommand : public Action {
public:
	ShellCommand(const std::string &cmd, std::shared_ptr<ControlMessage> request=nullptr);
	ShellCommand(const std::string &cmd, const std::string &prefix, std::shared_ptr<ControlMessage> request);

	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;

	/** Has this been invoked over s3tp? */
	bool is_s3tp();

protected:
	/** the running process */
	std::unique_ptr<Process> process;

	/** command to execute */
	std::string command;

	/** the request that lead to the execution of this shellcommand */
	std::shared_ptr<ControlMessage> request;

	/** prefix for command to execute */
	std::string cmdprefix;

	/** Is any command prefix set? */
	bool iscmdprefix;
};

} // horst
