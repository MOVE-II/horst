#pragma once

#include <string>

#include "state/state.h"

namespace horst {


/**
 * Available arguments for the program
 */
struct arguments {
	int port = 99;
	std::string socketpath = "/run/s3tp.socket";
	bool startmanual = false;
	State::leop_seq leop = State::leop_seq::DONE;
	uint16_t battery_treshold = 30;
	std::string scripts = "./scripts/";
};

}
