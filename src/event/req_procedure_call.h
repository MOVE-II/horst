#pragma once

#include "ctlmessage.h"


namespace horst {

/**
 * Control message to call a procedure.
 * Those are mainly some maneuvers the satellite supports.
 */
class ProcedureCallReq : public ControlMessage {
public:
	ProcedureCallReq(const std::string &name);

	void update(State &state) override;

private:
	std::string name;
};

} // horst
