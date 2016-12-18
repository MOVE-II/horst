#include "procedure_vm.h"

#include <iostream>

#include "instruction/instruction.h"
#include "procedure.h"


namespace horst {

ProcedureVM::ProcedureVM(uv_loop_t *loop, const Procedure *procedure,
                         procedure_cb_t on_done)
	:
	pc{0},
	loop{loop},
	procedure{procedure},
	state{vm_state::OK},
	on_done{on_done} {}


void ProcedureVM::step() {
	// when we were blocked, we now continue.
	if (this->state == vm_state::BLOCKED) {
		this->state = vm_state::OK;
	}

	while (this->state == vm_state::OK) {
		Instruction *next = procedure->get_instruction(this->pc);

		if (next == nullptr) {
			this->state = vm_state::ERROR;
			break;
		}

		this->pc += 1;

		// execute the instruction
		this->state = next->exec(this);
	}

	if (this->state == vm_state::DONE) {
		// if the stack is empty, return 0.
		vm_elem_t result = 0;

		// otherwise return the topmost stack element.
		if (this->stack.size() >= 1) {
			result = this->stack[this->sp()];
		}

		this->done(true, result);
	}
	else if (this->state == vm_state::ERROR) {
		// if the vm fails, return the program counter where it failed
		// as the pc was incremented after the instruction fetch,
		// it may have an offset of one.
		// TODO: purge the potential off-by-one.
		this->done(false, this->pc);
	}
}


uv_loop_t *ProcedureVM::get_loop() const {
	return this->loop;
}


size_t ProcedureVM::sp() const {
	return this->stack.size() - 1;
}


void ProcedureVM::done(bool success, int result) {
	if (this->on_done) {
		this->on_done(success, result);
	}
}

} // horst
