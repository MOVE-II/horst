#include "shell_command.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/ctlmessage.h"
#include "../process.h"
#include "../satellite.h"


namespace horst {

ShellCommand::ShellCommand(const std::string &cmd, std::shared_ptr<ControlMessage> request)
	:
	process{nullptr},
	command{cmd},
	request{request},
	cmdprefix{""},
	iscmdprefix{false} {}

ShellCommand::ShellCommand(const std::string &cmd, const std::string &prefix,
	    std::shared_ptr<ControlMessage> request)
	:
	process{nullptr},
	command{cmd},
	request{request},
	cmdprefix{prefix},
	iscmdprefix{true} {}


std::string ShellCommand::describe() const {
	std::stringstream ss;
	ss << "execute shell command: '" << this->command << "'";
	return ss.str();
}


bool ShellCommand::is_s3tp() {
	return (this->request) ? this->request->is_s3tp() : false;
}


void ShellCommand::perform(Satellite *sat, ac_done_cb_t done) {

	// Set default cmdprefix
	if (!this->iscmdprefix) {
		this->cmdprefix = sat->get_scripts_path();
		this->iscmdprefix = true;
	}

	this->process = std::make_unique<Process>(
		sat->get_loop(),
		this->cmdprefix + this->command,
		this->is_s3tp(),
		[this, done] (Process *, int64_t exit_code) {

			// Return exit code
			std::stringstream ss;
			ss << "[exit] " << exit_code << std::endl;

			// call callback that was set in the request
			if (this->request) {
				this->request->done(ss.str());
			}

			// call callback set in the action (this shellcommand)
			if (done) {
				done((exit_code == 0), this);
			}
		}
	);
}

} // horst
