#include "thm.h"

#include "../logger.h"
#include "../util.h"

namespace horst {

THM::THM()
	:
	all_temp{overall_temp::WARN} {}

std::vector<std::unique_ptr<Action>>
THM::transform_to(const THM & /*target*/) const {

	std::vector<std::unique_ptr<Action>> ret;

	return ret;
}

THM::overall_temp THM::str2temp(const char* name) {
	switch(util::str2int(name)) {
	case util::str2int("OK"):
		return THM::overall_temp::OK;
	case util::str2int("WARN"):
		return THM::overall_temp::WARN;
	case util::str2int("ALARM"):
		return THM::overall_temp::ALARM;
	default:
		LOG_WARN("Could not interpret '" + std::string(name) + "' as temperature level!");
		return THM::overall_temp::ALARM;
	}
}

} // horst
