#include "loadc.h"

#include "../procedure_vm.h"


namespace horst {
namespace inst {

LoadC::LoadC(int value)
	:
	value{value} {}


vm_state LoadC::exec(ProcedureVM *vm) const {
	vm->stack.push_back(this->value);
	return vm_state::OK;
}

}} // horst::inst
