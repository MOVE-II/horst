#include "procedure.h"

#include "instruction/instruction.h"


namespace horst {

Procedure::Procedure(std::vector<std::unique_ptr<Instruction>> &&code)
	:
	code{std::move(code)}{}


Instruction *Procedure::get_instruction(pc_t at) const {
	if (at >= this->code.size()) {
		return nullptr;
	}
	return this->code[at].get();
}

} // horst
