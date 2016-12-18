#pragma once

#include "instruction.h"


namespace horst {
namespace inst {

/**
 * Halt the execution regularly.
 * Return the topmost stack element.
 */
class Halt : public Instruction {
public:
	Halt();

	vm_state exec(ProcedureVM *vm) const override;
};

}} // horst::inst
