#pragma once

namespace horst {

/**
 * States of the vm type.
 */
enum class vm_state {
	BLOCKED,
	OK,
	DONE,
	ERROR,
};

} // horst
