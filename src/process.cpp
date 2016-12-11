#include "process.h"

#include <iostream>
#include <cstring>


namespace horst {

Process::Process(uv_loop_t *loop, const std::string &cmd) {
	std::cout << "[process] created for '" << cmd << "'" << std::endl;

	this->handle.data = this;

	const char *proc_args[4] = {
		"sh", "-c", cmd.c_str(), nullptr
	};

	this->options = {};
	this->handle = {};

	// fuck you c++. srsly.
	char *args_cpy[4];
	memcpy(&args_cpy, &proc_args, sizeof(args_cpy));

	options.exit_cb = [] (uv_process_t *req,
	                      int64_t exit_status,
	                      int term_signal) {

		Process *this_ = (Process *) req->data;
		uv_close((uv_handle_t*) req, nullptr);

		this_->exited(exit_status, term_signal);
	};

	this->options.file = "sh";
	this->options.args = args_cpy;

	int r;
	if ((r = uv_spawn(loop, &this->handle, &this->options))) {
		std::cout << "[process] failed spawning: "
		          << uv_strerror(r) << std::endl;
	}
	else {
		std::cout << "[process] launched process with id "
		          << this->handle.pid << std::endl;
	}
}

// TODO: this could be extended to capture the output of the process
// https://nikhilm.github.io/uvbook/processes.html#child-process-i-o


void Process::exited(int64_t exit_code, int term_signal) {
	std::cout << "[process] exited with "
	          << exit_code << " by signal " << term_signal << std::endl;
	// TODO: remove this process from the process list.
}

} // horst
