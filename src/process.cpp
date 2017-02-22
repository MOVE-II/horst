#include "process.h"

#include <iostream>
#include <cstring>

#include "logger.h"

namespace horst {

Process::Process(uv_loop_t *loop, const std::string &cmd,
                 proc_exit_cb_t on_exit)
	:
	cmd{cmd},
	on_exit{on_exit},
	exit_code{-1} {

	LOG_INFO("[process] created for '"+ std::string(cmd) +"'");

	// TODO: implement proper sh lexing so we don't need to run sh -c
	const char *proc_args[4] = {
		"sh", "-c", cmd.c_str(), nullptr
	};

	this->options = {};
	this->handle = {};

	// fuck you c++. srsly.
	char *args_cpy[4];
	memcpy(&args_cpy, &proc_args, sizeof(args_cpy));

	this->handle.data = this;
	this->options.exit_cb = [] (uv_process_t *req,
	                            int64_t exit_status,
	                            int /*term_signal*/) {

		Process *this_ = (Process *) req->data;

		this_->exit_code = exit_status;

		// close the process handle,
		// call the actual exited callback after that.
		// this is because the exited() function will free the memory.
		uv_close(
			(uv_handle_t*) req,
			[] (uv_handle_t *handle) {

				Process *this_ = (Process *) handle->data;
				this_->exited();
			}
		);
	};

	this->options.file = "sh";
	this->options.args = args_cpy;

	int r;
	if ((r = uv_spawn(loop, &this->handle, &this->options))) {
		LOG_WARN("[process] failed spawning: "+ std::string(uv_strerror(r)));

		this->exit_code = -1;

		// TODO unsure if we need to close the handle here as well.
		// we only close it in the options.exit_cb at the moment.
		// dunno what happens if the spawn fails.

		this->exited();
	}
	else {
		LOG_INFO("[process] launched process with id : "+ std::to_string(this->handle.pid));
	}
}


// TODO: this could be extended to capture the output of the process
// https://nikhilm.github.io/uvbook/processes.html#child-process-i-o


void Process::exited() {
	// call the process exit callback.
	if (this->on_exit) {
		this->on_exit(this, this->exit_code);
	}
}

} // horst
