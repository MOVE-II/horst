#pragma once

#include <string>

namespace horst {

enum s3tp_horst_type {
	NONE, /* Status information only */
	SIMPLE, /* Status information only */
	OUTPUT, /* Return full output of command at once */
	INTERACTIVE, /* Return output of command as is */
};

struct s3tp_horst_header {
	size_t length;
	bool complete;
	s3tp_horst_type type;
};


} // horst
