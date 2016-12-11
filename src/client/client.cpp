#include "client.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "../event/ctlmessage.h"
#include "../satellite.h"


namespace horst {

Client::Client(Satellite *satellite)
	:
	satellite{satellite},
	buf{std::make_unique<char[]>(this->max_buf_size)},
	buf_used{0} {}


void Client::data_received(const char *data, size_t size) {
	if (this->buf_used + size >= this->max_buf_size) {
		std::cout << "[client] receive buffer too full, closing..."
		          << std::endl;
		this->close();
		return;
	}

	// The following "few" lines implement in C++ what Python does:
	//   ... buf = bytearray() ...
	//   buf.extend(data)
	//   npos = buf.rfind(b'\n')
	//   for line in buf[:npos].split(b'\n'):
	//       # thing with line
	//   del buf[:npos]
	size_t ncpy = std::min(size, (this->max_buf_size - this->buf_used - 1));

	// copy data at the beginning
	std::memcpy(&this->buf[this->buf_used], data, ncpy);

	this->buf[this->buf_used + ncpy] = '\0';
	this->buf_used += ncpy;

	char split_at = '\n';

	std::string buf_str{this->buf.get()};
	size_t npos = buf_str.rfind(split_at);

	if (npos == std::string::npos) {
		// no newline found, just wait for more data.
		return;
	}

	std::stringstream lines{buf_str.substr(0, npos)};
	std::string part;
	std::vector<std::string> seglist;

	while (lines.good()) {
		std::getline(lines, part, split_at);
		seglist.push_back(part);
	}

	std::memmove(this->buf.get(), &this->buf[npos + 1],
	             this->buf_used - npos + 1);

	this->buf_used -= npos + 1;

	for (auto &entry : seglist) {
		auto cmd = ControlMessage::parse(entry);

		if (cmd.get() != nullptr) {
			// handle each command in the event handler
			this->satellite->on_event(std::move(cmd));
			this->send("ack\n");
		}
		else {
			// ignore the command.
		}
	}
}

void Client::send(const char *text) {
	this->send(text, strlen(text));
}

} // horst
