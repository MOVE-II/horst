#pragma once

#include <string>
#include <uv.h>


namespace horst {

/**
 * A subprocess that is called by horst.
 */
class Process {
public:
	Process(uv_loop_t *loop, const std::string &cmd);

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
	void exited(int64_t exit_code, int term_signal);

	uv_process_t handle;
	uv_process_options_t options;
};

} // horst
