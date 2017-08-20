#pragma once

#include <functional>
#include <string>
#include <uv.h>


namespace horst {

class Process;
class S3TPServer;

/**
 * Callback type for process exit notifications.
 */
using proc_exit_cb_t = std::function<void(Process *, int64_t)>;


/**
 * A subprocess that is called by horst.
 */
class Process {
public:
	Process(uv_loop_t *loop, const std::string &cmd, bool s3tp,
	        proc_exit_cb_t on_exit=nullptr);

	// no moves and copies
	Process(Process &&other) = delete;
	Process(const Process &other) = delete;
	Process &operator =(Process &&other) = delete;
	Process &operator =(const Process &other) = delete;

	virtual ~Process() = default;

	/**
	 * Kill process
	 */
	void kill();

	/**
	 * Start pushing output to S3TP
	 */
	void start_output();

	/**
	 * Do no longer push output to S3TP (will buffer instead)
	 */
	void stop_output();

	/**
	 * Forward data to stdin of process
	 */
	void input(char*, size_t);

	/**
	 * Closes the input-pipe if it exists
	 */
	void close_input();

protected:
	/**
	 * called when the process exited.
	 */
	void exited();

	/** Process handle */
	uv_process_t handle;

	/** Options for spawned process */
	uv_process_options_t options;

	/** command that is run by this process */
	std::string cmd;

	/** callback to be invoked upon exit */
	proc_exit_cb_t on_exit;

	/** process exit code. is -1 if the processes did not exit yet. */
	int64_t exit_code;

	/** Last received signal */
	int signal;

	/** Pipe to redirect stdout of child process */
	uv_pipe_t pipe_out;

	/** Pipe to redirect stderr of child process */
	uv_pipe_t pipe_err;

	/** Flag is true when pipe_in is open */
	bool has_in;

	/** Pipe to redirect stdin of child process */
	uv_pipe_t pipe_in;

	/** Callback for pipe to allocate buffer */
	static void alloc_buffer(uv_handle_t*, size_t, uv_buf_t*);

	/** Callback for pipe to read stdout/err */
	static void read_callback(uv_stream_t*, ssize_t, const uv_buf_t*);
};

} // horst
