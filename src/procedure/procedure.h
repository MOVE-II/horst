#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "instruction/instruction.h"


namespace horst {

/**
 * Type for the program counter.
 */
using pc_t = uint64_t;

class Procedure {
public:
	Procedure(std::vector<std::unique_ptr<Instruction>> &&code);

	virtual ~Procedure() = default;

	Instruction *get_instruction(pc_t at) const;

protected:
	/**
	 * "machine" code for the procedure
	 */
	std::vector<std::unique_ptr<Instruction>> code;
};

} // horst
