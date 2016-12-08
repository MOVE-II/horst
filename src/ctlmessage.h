#pragma once

#include <string>


namespace horst {

/**
 * types of earth control messages
 */
enum class ctl_msg_t {
	RUN,
	STATE,
	EXEC,
	START,
	STOP,
	RESTART,
	STATUS
};


/**
 * Control messages via s3tp from earth.
 */
class ControlMessage {
public:
	ControlMessage(const std::string &msg);
	virtual ~ControlMessage() = default;

	bool is_valid();
	ctl_msg_t get_type();
	const std::string &get_procedure_name() const;
	const std::string &get_subsystem_id() const;
	const std::string &get_shell_command() const;

private:
	std::string procedure_name;
	std::string subsystem_id;
	std::string shell_command;
	ctl_msg_t type;
	bool valid;
};

} // horst
