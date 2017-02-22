#pragma once

#include <string>

namespace horst {


/**
 * available arguments for the program
 */
struct arguments {
	bool verbose = false;
	int port = 9001;
	std::string scripts = "./scripts/";
};

}
