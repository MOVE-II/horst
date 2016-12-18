#pragma once

#include <uv.h>

#include "procedure.h"
#include "vm_state.h"


namespace horst {

/** element type used for each stack element */
using vm_elem_t = int;

/** callback type when the procedure is finished */
using procedure_cb_t = std::function<void(bool success, vm_elem_t result)>;


/**
 * A mini virtual machine implementation to execute procedures.
 */
class ProcedureVM {
public:
	ProcedureVM(uv_loop_t *loop, const Procedure *procedure,
	            procedure_cb_t on_done=nullptr);

	virtual ~ProcedureVM() = default;

	/**
	 * Perform the next step of the procedure.
	 */
	void step();

	/** return the event loop */
	uv_loop_t *get_loop() const;

	/** return the stack pointer */
	size_t sp() const;

	/** called when the vm executed the procedure successfully */
	void done(bool success, int result);

	/** Program counter for this run. */
	pc_t pc;

	/** the stack of the machine */
	std::vector<vm_elem_t> stack;

protected:
	/** Loop this procedure is executed in. */
	uv_loop_t *loop;

	/** The procedure executed by this runner. */
	const Procedure *procedure;

	/** State of the execution */
	vm_state state;

	/** called when the procedure finished */
	procedure_cb_t on_done;
};

} // horst
