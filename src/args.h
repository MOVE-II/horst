#pragma once

#include <string>

namespace horst {


/**
 * available arguments for the program
 */
struct arguments {
	int port = 9001;
	bool startmanual = false;
	uint16_t battery_treshold = 5000;
	std::string scripts = "./scripts/";
};

}
