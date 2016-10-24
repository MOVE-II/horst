#include "horst.h"

#include <iostream>

#include "error.h"

namespace horst {

int run(int argc, char **argv) {
	try {
		std::cout << "Horst launching..." << std::endl;
		return 0;
	}
	catch (Error &error) {
		std::cout << "### Horst internal error ###" << std::endl;
		std::cout << error.what() << std::endl;
		return 1;
	}
	catch (std::exception &error) {
		std::cout << "### Fatal C++ error ###" << std::endl;
		std::cout << error.what() << std::endl;
		return 2;
	}
}

} // horst

int main(int argc, char **argv) {
	return horst::run(argc, argv);
}
