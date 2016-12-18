#pragma once

#include <cstdint>

#include "instruction.h"


namespace horst {
namespace inst {

/**
 * Sleep instruction that pauses the vm for some rough timespan
 * in milliseconds.
 */
class Sleep : public Instruction {
public:
	/**
	 * create a sleep instruction that sleeps for n milliseconds
	 */
	Sleep(uint64_t time);

	vm_state exec(ProcedureVM *vm) const override;

protected:

	/** target time to sleep in milliseconds */
	uint64_t time;
};

}} // horst::inst
