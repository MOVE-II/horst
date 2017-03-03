#include "ctlmessage.h"

#include "../logger.h"
#include "req_daemon_control.h"
#include "req_procedure_call.h"
#include "req_shell_command.h"


namespace horst {

ControlMessage::ControlMessage()
	:
	on_done{nullptr} {}


std::shared_ptr<ControlMessage>
ControlMessage::parse(const std::string &msg) {

	std::shared_ptr<ControlMessage> ret = nullptr;

	LOG_DEBUG("[event] Parsing control message: " + std::string(msg));
	std::size_t first_space = msg.find(" ");

	if (first_space == std::string::npos) {
		LOG_DEBUG("[event] No parameters given to command");
		return ret;
	}

	std::string command_type = msg.substr(0, first_space);
	std::string rest         = msg.substr(first_space + 1);

	if (rest.size() >= 1 && rest.back() == '\n') {
		rest = rest.substr(0, rest.size() - 1);
	}

	if (rest.size() == 0) {
		LOG_DEBUG("[event] Parameters have a length of 0");
		return ret;
	}

	if (command_type == "run") {
		ret = std::make_unique<ProcedureCallReq>(rest);
	}
	else if (command_type == "exec") {
		ret = std::make_unique<ShellCommandReq>(rest);
	}
	else if (command_type == "status") {
		ret = std::make_unique<DaemonControlReq>(
			daemon_ctl_action_t::STATUS,
			rest
		);
	}
	else if (command_type == "start") {
		ret = std::make_unique<DaemonControlReq>(
			daemon_ctl_action_t::START,
			rest
		);
	}
	else if (command_type == "stop") {
		ret = std::make_unique<DaemonControlReq>(
			daemon_ctl_action_t::STOP,
			rest
		);
	}
	else if (command_type == "restart") {
		ret = std::make_unique<DaemonControlReq>(
			daemon_ctl_action_t::RESTART,
			rest
		);
	}
	// TODO: more commands like "report" => StatusReport of the satellite
	else {
		LOG_WARN("[event] Unhandled command: " + std::string(command_type));
	}

	return ret;
}


bool ControlMessage::is_fact() const {
	// control messages are never facts.
	// it's something the ground station requests.
	return false;
}


void ControlMessage::call_on_complete(done_cb_t on_done) {
	this->on_done = on_done;
}


void ControlMessage::done(const std::string &result) {
	if (this->on_done) {
		this->on_done(result);
	}
}

}  // horst
