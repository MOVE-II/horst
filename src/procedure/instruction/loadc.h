#pragma once

#include "instruction.h"


namespace horst {
namespace inst {

/**
 * Push a constant value onto the stack.
 */
class LoadC : public Instruction {
public:
	LoadC(int value);

	vm_state exec(ProcedureVM *vm) const override;

protected:
	/** value to push on the stack */
	int value;
};

}} // horst::inst
