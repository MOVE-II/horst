#include "procedure_manager.h"

#include "instruction/instructions.h"


namespace horst {

ProcedureManager::ProcedureManager() {

	// here, register all procedures available.
	// one could write a parser and compiler to create those
	// instructions from some small programming language.
	// yes, we could just have used python.

	std::vector<std::unique_ptr<Instruction>> vec;
	vec.push_back(std::make_unique<inst::Sleep>(2000));
	vec.push_back(std::make_unique<inst::NOP>());
	vec.push_back(std::make_unique<inst::NOP>());
	vec.push_back(std::make_unique<inst::LoadC>(42));
	vec.push_back(std::make_unique<inst::Halt>());

	this->procedures.emplace("test", std::move(vec));
}


const Procedure *
ProcedureManager::get_procedure(const std::string &name) const {
	auto loc = this->procedures.find(name);
	if (loc == std::end(this->procedures)) {
		return nullptr;
	} else {
		return &loc->second;
	}
}

} // horst
