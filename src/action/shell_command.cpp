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
	request{request} {}


std::string ShellCommand::describe() const {
	std::stringstream ss;
	ss << "execute shell command: '" << this->command << "'";
	return ss.str();
}


void ShellCommand::perform(Satellite *sat, ac_done_cb_t done) {
	this->process = std::make_unique<Process>(
		sat->get_loop(),
		this->command,
		[this, done] (Process *, int64_t exit_code) {

			// TODO: if we wanna provide the process' output someday,
			//       provide it here instead of this stupid exit code.
			std::stringstream ss;
			ss << "request exited with " << exit_code << std::endl;

			// call callback that was set in the request
			if (this->request) {
				this->request->done(ss.str());
			}

			// call callback set in the action (this shellcommand)
			if (done) {
				done(this);
			}
		}
	);
}

} // horst
