#include "horst.h"

#include <getopt.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "error.h"
#include "log.h"
#include "satellite.h"
#include "version.h"

/**
 * Contains all components of horst, the move-II state controller.
 *
 * (c) 2016 Jonas Jelten <jelten@in.tum.de>
 * released under GNU GPLv3 or later.
 */
namespace horst {


/**
 * main program arguments, declared in header as extern.
 */
arguments args;


void show_help(const char *progname) {
	std::cout << "horst\n"
	          << "MOVE-II satellite manager\n"
	          << "\n"
	          << "usage: " << progname << "[-v] [-q] [-h|--help] [-p|--port=LISTENPORT]\n"
	          << std::endl;
}


arguments parse_args(int argc, char **argv) {
	int c;
	arguments args;

	while (true) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help",    no_argument,       0, 'h'},
			{"port",    required_argument, 0, 'p'},
			{"quiet",   no_argument,       0, 'q'},
			{"verbose", no_argument,       0, 'v'},
			{0,         0,                 0,  0 }
		};

		c = getopt_long(argc, argv, "hp:qv",
		                long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			// get long_options[option_index].name
			// if (optarg)
			// printf(" with arg %s", optarg);
			break;

		case 'v':
			args.verbose = true;
			break;

		case 'q':
			args.verbose = false;
			break;

		case 'p':
			args.port = std::stoi(optarg);
			break;

		case 'h':
			show_help(argv[0]);
			exit(0);
			break;

		case '?':
			break;

		default:
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	if (optind < argc) {
		// we have arguments left at argv[optind] etc
	}

	return args;
}


int run(int argc, char **argv) {
	try {
		// set the global args
		args = parse_args(argc, argv);

		std::cout << "Starting Horst " << VERSION << std::endl;

		Satellite move2{args};

		return move2.run();
	}
	catch (Error &error) {
		std::cout << "### Horst internal error ###" << std::endl;
		std::cout << error.what() << std::endl;
		std::cout << error << std::endl;
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
