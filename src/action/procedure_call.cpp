#include "procedure_call.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../event/ctlmessage.h"
#include "../procedure/procedure_vm.h"
#include "../satellite.h"


namespace horst {

ProcedureCall::ProcedureCall(const std::string &name,
                             std::shared_ptr<ControlMessage> request)
	:
	vm{nullptr},
	name{name},
	request{request} {}


std::string ProcedureCall::describe() const {
	std::stringstream ss;
	ss << "run procedure: '" << this->name << "'";
	return ss.str();
}


void ProcedureCall::perform(Satellite *sat, ac_done_cb_t done) {
	const Procedure *procedure = sat->get_procedure(this->name);

	if (procedure == nullptr) {
		std::stringstream ss;
		ss << "tried to call unknown procedure: "
		   << this->name << std::endl;

		this->request->done(ss.str());
		if (done) {
			done(false, this);
		}
		return;
	}

	this->vm = std::make_unique<ProcedureVM>(
		sat->get_loop(),
		procedure,
		[this, done] (bool success, vm_elem_t result) {

			std::stringstream ss;
			ss << "procedure '"
			   << this->name;

			if (success) {
				ss << "' finished with: "
				   << result;
			} else {
				ss << "' failed at: " << result;
			}

			ss << std::endl;

			// call callback that was set in the request
			if (this->request) {
				this->request->done(ss.str());
			}

			// call callback set up in the action performing
			if (done) {
				done(success, this);
			}
		}
	);

	this->vm->step();
}

} // horst
