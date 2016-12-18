#include "req_procedure_call.h"

#include <tuple>

#include "../state/state.h"


namespace horst {

ProcedureCallReq::ProcedureCallReq(const std::string &name)
	:
	name{name} {}

void ProcedureCallReq::update(State &state) {
	state.computer.procedures.push_back(
		std::make_tuple(
			this->name,
			std::static_pointer_cast<ControlMessage>(this->shared_from_this())
		)
	);

}

} // horst
