#include "adcs.h"


namespace horst {

ADCS::ADCS()
	:
	pointing{adcs_state::NONE},
	requested{adcs_state::NONE} {}

std::vector<std::unique_ptr<Action>>
ADCS::transform_to(const ADCS & /*target*/) const {
	std::vector<std::unique_ptr<Action>> ret;
	return ret;
}

} // horst
