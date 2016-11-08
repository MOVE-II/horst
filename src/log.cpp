#include "log.h"

#include <iostream>

#include "horst.h"


namespace horst {


void log(const std::string &msg, bool verbose) {
	if (verbose and not args.verbose) {
		return;
	}

	std::cout << msg << std::endl;
}


} // horst
