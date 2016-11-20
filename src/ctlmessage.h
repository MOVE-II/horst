#pragma once

#include <string>


namespace horst {

/**
 * Control message interpreting.
 */
class ControlMessage {
	enum class type {
		start
	};

public:
	ControlMessage(const std::string &msg);
	virtual ~ControlMessage() = default;
};


} // horst
