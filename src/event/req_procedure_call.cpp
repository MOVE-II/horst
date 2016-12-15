#include "req_procedure_call.h"


namespace horst {

ProcedureCallReq::ProcedureCallReq(const std::string &name)
	:
	name{name} {}


void ProcedureCallReq::update(State &/*state*/) {
	// TODO: enqueue procedure
}

} // horst
