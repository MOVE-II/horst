#pragma once

#include <string>
#include <unordered_map>

#include "procedure.h"


namespace horst {

/**
 * Container for finding and handling procedures.
 */
class ProcedureManager {
public:
	ProcedureManager();

	virtual ~ProcedureManager() = default;

	/**
	 * Fetch a procedure with the given name.
	 *
	 * Beware: If two procedures are executed at the same time
	 *         and store state in their instruction objects,
	 *         things will fail horribly!
	 *      -> never store state in instruction objects
	 *         or wrap them as steps which then return instructions
	 *         that are instanced for each execution separately.
	 */
	const Procedure *get_procedure(const std::string &name) const;

protected:
	std::unordered_map<std::string, Procedure> procedures;
};

}  // horst
