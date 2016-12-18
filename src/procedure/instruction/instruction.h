#pragma once

#include "../vm_state.h"


namespace horst {

class ProcedureVM;


class Instruction {
public:
	Instruction();

	virtual ~Instruction() = default;

	/**
	 * perform this instruction in the given vm
	 *
	 * if the execution performs io, this function must
	 * call vm->step() when the io is done.
	 *
	 * in this case, the function returns true,
	 * so the execution loop is not continued automatically.
	 *
	 * @return: true if vm->step() is called internally,
	 *          false if the vm shall continue on its own.
	 */
	virtual vm_state exec(ProcedureVM *vm) const = 0;
};

} // horst
