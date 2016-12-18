#include "sleep.h"

#include <uv.h>

#include "../procedure_vm.h"


namespace horst {
namespace inst {

Sleep::Sleep(uint64_t time)
	:
	time{time} {}


vm_state Sleep::exec(ProcedureVM *vm) const {
	uv_timer_t *timer = new uv_timer_t;
	uv_timer_init(vm->get_loop(), timer);

	timer->data = vm;

	int err = uv_timer_start(
		timer,
		[] (uv_timer_t *handle) {
			ProcedureVM *vm = (ProcedureVM *)handle->data;

			uv_close(
				(uv_handle_t *)handle,
				[] (uv_handle_t *handle) {
					delete handle;
				}
			);

			// the vm may now continue.
			vm->step();
		},
		this->time,  // < time to wait
		0            // < don't repeat the timer
	);

	if (err) {
		return vm_state::ERROR;
	}

	// a sleep always blocks!
	return vm_state::BLOCKED;
}

}} // horst::inst
