#pragma once

#include <functional>
#include <string>
#include <uv.h>


namespace horst {

class Process;

/**
 * Callback type for process exit notifications.
 */
using proc_exit_cb_t = std::function<void(Process *, int64_t)>;


/**
 * A subprocess that is called by horst.
 */
class Process {
public:
	Process(uv_loop_t *loop, const std::string &cmd,
	        proc_exit_cb_t on_exit=nullptr);

	// no moves and copies
	Process(Process &&other) = delete;
	Process(const Process &other) = delete;
	Process &operator =(Process &&other) = delete;
	Process &operator =(const Process &other) = delete;

	virtual ~Process() = default;

protected:
	/**
	 * called when the process exited.
	 */
	void exited();

	uv_process_t handle;
	uv_process_options_t options;

	/** command that is run by this process */
	std::string cmd;

	/** callback to be invoked upon exit */
	proc_exit_cb_t on_exit;

	/** process exit code. is -1 if the processes did not exit yet. */
	int64_t exit_code;
};

} // horst
