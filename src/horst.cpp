#include "horst.h"

#include <getopt.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "error.h"
#include "satellite.h"
#include "util.h"

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
	          << "usage: " << progname << "[-b|--battery=TRESHOLD] [-h|--help] [-m|--manual] [-p|--port=LISTENPORT] [-s|--scripts=PATH]\n"
	          << std::endl;
}


arguments parse_args(int argc, char **argv) {
	int c;
	arguments args;

	while (true) {
		int option_index = 0;
		static struct option long_options[] = {
			{"battery", required_argument, 0, 'b'},
			{"help",    no_argument,       0, 'h'},
			{"manual",  no_argument,       0, 'm'},
			{"port",    required_argument, 0, 'p'},
			{"scripts", required_argument, 0, 's'},
			{0,         0,                 0,  0 }
		};

		c = getopt_long(argc, argv, "b:hmp:s:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			// get long_options[option_index].name
			// if (optarg)
			// printf(" with arg %s", optarg);
			break;

		case 'b':
			args.battery_treshold = std::stoi(optarg);
			break;

		case 'h':
			show_help(argv[0]);
			exit(0);
			break;

		case 'm':
			args.startmanual = true;
			break;

		case 'p':
			args.port = std::stoi(optarg);
			break;

		case 's':
			args.scripts = optarg;
			break;

		case '?':
			LOG_WARN("Something is wrong with the parameters!");
			show_help(argv[0]);
			exit(1);

		default:
			LOG_WARN("Invalid parameter " + std::to_string(c));
			show_help(argv[0]);
			exit(1);
		}
	}

	return args;
}


int run(int argc, char **argv) {
	LOG_INFO("Starting Horst " + std::string(VERSION));
	try {
		// set the global args
		args = parse_args(argc, argv);

		// Check leop status
		switch (util::exec(args.scripts + "checkleop.sh")) {
		case 0:
			// LEOP done
			LOG_INFO("LEOP is done already");
			args.leop = State::leop_seq::DONE;
			break;
		case 1:
			// LEOP not done yet
			LOG_INFO("LEOP is not done yet!");
			args.leop = State::leop_seq::UNDEPLOYED;
			break;
		default:
			// ERROR
			LOG_ERROR(11, "Failed to check leop status!");
			return 3;
		}

		// Run startup script
		if (util::exec(args.scripts + "startup.sh") != 0) {
			LOG_WARN("Startup script failed!");
		}

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
