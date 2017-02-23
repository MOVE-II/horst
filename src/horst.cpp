#include "horst.h"

#include <getopt.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "error.h"
#include "logger.h"
#include "satellite.h"

/**
 * Logger settings
 */
const bool TIMESTAMP_ENABLED = false;
const std::string SUB_COMPONENT = "HORST";

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
			{"scripts", required_argument, 0, 's'},
			{0,         0,                 0,  0 }
		};

		c = getopt_long(argc, argv, "hp:qvs:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			// get long_options[option_index].name
			// if (optarg)
			// printf(" with arg %s", optarg);
			break;

		case 'p':
			args.port = std::stoi(optarg);
			break;

		case 's':
			args.scripts = optarg;
			break;

		case 'h':
			show_help(argv[0]);
			exit(0);
			break;

		case '?':
			break;

		default:
			LOG_WARN("Invalid parameter " + std::to_string(c));
		}
	}

	if (optind < argc) {
		// we have arguments left at argv[optind] etc
	}

	return args;
}


int run(int argc, char **argv) {
	LOG_INFO("Starting Horst " + std::string(VERSION));
	try {
		// set the global args
		args = parse_args(argc, argv);

		Satellite move2{args};

		return move2.run();
	}
	catch (Error &error) {
		LOG_CRITICAL(1, std::string("Internal error! " + std::string(error.what())));
		return 1;
	}
	catch (std::exception &error) {
		LOG_CRITICAL(2, std::string("Fatal C++ error! " + std::string(error.what())));
		return 2;
	}
}

} // horst

int main(int argc, char **argv) {
	return horst::run(argc, argv);
}
