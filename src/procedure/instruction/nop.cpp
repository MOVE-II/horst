#include "nop.h"


namespace horst {
namespace inst {

NOP::NOP() {}


vm_state NOP::exec(ProcedureVM * /*vm*/) const {
	return vm_state::OK;
}


}} // horst::inst
