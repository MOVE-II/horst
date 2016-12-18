#pragma once

#include <memory>

#include "action.h"

namespace horst {

class ControlMessage;
class ProcedureVM;


class ProcedureCall : public Action {
public:
	ProcedureCall(const std::string &name,
	              std::shared_ptr<ControlMessage> request=nullptr);

	std::string describe() const override;
	void perform(Satellite *satellite, ac_done_cb_t done) override;

protected:
	/** vm where the procedure is executed in */
	std::unique_ptr<ProcedureVM> vm;

	/** procedure name to call */
	std::string name;

	/** the request that lead to the call of the procedure */
	std::shared_ptr<ControlMessage> request;
};

} // horst
