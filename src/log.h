#pragma once

#include <string>

namespace horst {

/**
 * Log a message.
 * If verbose is true, only log it if running in verbose mode.
 */
void log(const std::string &msg, bool verbose=false);

}
