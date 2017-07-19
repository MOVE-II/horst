#include "process.h"
#include "horst.h"
#include "satellite.h"

#include <iostream>
#include <cstring>
#include <s3tp/core/Logger.h>


namespace horst {

Process::Process(uv_loop_t *loop, const std::string &cmd, bool s3tp,
                 proc_exit_cb_t on_exit)
	:
	cmd{cmd},
	on_exit{on_exit},
	exit_code{-1} {
	int r;

	LOG_INFO("[process] created for '"+ std::string(cmd) +"'" + (s3tp ? " from s3tp" : ""));

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

		// Close pipe handles
		// Only do this after closing the handle
		if (this_->options.stdio_count == 3) {
			uv_close(
				(uv_handle_t*) &this_->pipe_stdout,
				[] (uv_handle_t *) {}
			);
			uv_close(
				(uv_handle_t*) &this_->pipe_stderr,
				[] (uv_handle_t *) {}
			);
		}
	};

	this->options.file = "sh";
	this->options.args = args_cpy;

	// Redirect output for s3tp
	uv_stdio_container_t child_stdio[3];
	if (s3tp) {
		uv_pipe_init(loop, &this->pipe_stdout, 1);
		uv_pipe_init(loop, &this->pipe_stderr, 1);

		this->options.stdio_count = 3;
		child_stdio[0].flags = UV_IGNORE;
		child_stdio[1].flags = (uv_stdio_flags) (UV_CREATE_PIPE | UV_WRITABLE_PIPE);
		child_stdio[1].data.stream = (uv_stream_t *) &this->pipe_stdout;
		child_stdio[2].flags = (uv_stdio_flags) (UV_CREATE_PIPE | UV_WRITABLE_PIPE);
		child_stdio[2].data.stream = (uv_stream_t *) &this->pipe_stderr;
		this->options.stdio = child_stdio;
	}

	if ((r = uv_spawn(loop, &this->handle, &this->options))) {
		LOG_WARN("[process] failed spawning: "+ std::string(uv_strerror(r)));

		this->exit_code = -1;

		// TODO unsure if we need to close the handle here as well.
		// we only close it in the options.exit_cb at the moment.
		// dunno what happens if the spawn fails.

		this->exited();
	} else {
		if (s3tp) {
			uv_read_start((uv_stream_t*)&this->pipe_stdout, alloc_buffer, [](uv_stream_t*, ssize_t nread, const uv_buf_t* buf) {
				if (nread > 0) {
					if (nread + 1 > (ssize_t) buf->len) {
						if (buf->base != NULL)
							free(buf->base);
						return;
					}
					buf->base[nread] = '\0';
					satellite->get_s3tp()->send(buf->base, nread);
					LOG_DEBUG("[process] stdout: " + std::string(buf->base));
				} else {
					if (nread == UV_EOF) {
						LOG_DEBUG("[process] EOF");
					}
				}
				if (buf->base != NULL)
				    free(buf->base);
			});
			uv_read_start((uv_stream_t*)&this->pipe_stderr, alloc_buffer, [](uv_stream_t*, ssize_t nread, const uv_buf_t* buf) {
				if (nread > 0) {
					if (nread + 1 > (ssize_t) buf->len) {
						if (buf->base != NULL)
							free(buf->base);
						return;
					}
					buf->base[nread] = '\0';
					satellite->get_s3tp()->send(buf->base, nread);
					LOG_DEBUG("[process] stderr: " + std::string(buf->base));
				} else {
					if (nread == UV_EOF) {
						LOG_DEBUG("[process] EOF");
					}
				}
				if (buf->base != NULL)
				    free(buf->base);
			});
		}
		LOG_INFO("[process] launched process with id : "+ std::to_string(this->handle.pid));
	}
}

void Process::alloc_buffer(uv_handle_t*, size_t suggested_size, uv_buf_t* buf) {
	buf->base = (char*) malloc(suggested_size);
	buf->len = suggested_size;
}

void Process::exited() {
	// call the process exit callback.
	if (this->on_exit) {
		this->on_exit(this, this->exit_code);
	}
}

} // horst
