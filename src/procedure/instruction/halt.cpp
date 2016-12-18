#include "halt.h"


namespace horst {
namespace inst {

Halt::Halt() {}


vm_state Halt::exec(ProcedureVM * /*vm*/) const {
	// set the vm to done state.
	return vm_state::DONE;
}

}} // horst::inst
