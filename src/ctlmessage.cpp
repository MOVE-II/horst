#include "ctlmessage.h"

#include <iostream>


namespace horst {

ControlMessage::ControlMessage(const std::string &msg) {
	std::cout << "create control message: " << msg << std::endl;
}

} // horst
