#pragma once

#include "instruction.h"


namespace horst {
namespace inst {

/**
 * Do nothing.
 */
class NOP : public Instruction {
public:
	NOP();

	vm_state exec(ProcedureVM *vm) const override;
};

}} // horst::inst
