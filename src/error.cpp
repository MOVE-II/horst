#include "error.h"

namespace horst {

Error::Error(const std::string &msg)
	:
	message{msg} {}


const char *Error::what() const noexcept {
	return this->message.c_str();
}

} // horst
