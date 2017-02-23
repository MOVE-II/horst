#include "payload.h"


namespace horst {

Payload::Payload()
	:
	daemon{daemon_state::OFF} {}

std::vector<std::unique_ptr<Action>>
Payload::transform_to(const Payload & /*target*/) const {
	std::vector<std::unique_ptr<Action>> ret;
	return ret;
}

} // horst
