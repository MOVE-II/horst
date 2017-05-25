#pragma once

#include <memory>

#include "args.h"

extern const char* VERSION;

namespace horst {

/**
 * main program arguments.
 */
extern arguments args;

/**
 * Global reference to satellite object
 */
extern std::unique_ptr<Satellite> satellite;

}
