#pragma once

#include "ctlmessage.h"


namespace horst {

/**
 * This is just a shell command executed directly on the satellite.
 */
class ShellCommandReq : public ControlMessage {
public:
	ShellCommandReq(const std::string &command);

	/** return the command to be executed */
	const std::string &get_cmd() const;

	void update(State &state) override;

private:
	std::string cmd;
};

} // horst
