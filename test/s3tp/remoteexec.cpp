#include <s3tp/connector/S3tpChannelSync.h>
#include <fstream>

#include "../../src/server/s3tp_protocol.h"

const std::string SUB_COMPONENT = "S3TP";
const bool TIMESTAMP_ENABLED = false;

const uint8_t PORT_HORST = 99;
const uint8_t PORT_LOCAL = 4000;
char* SOCKETPATH = "/tmp/s3tp4000";


const size_t bufferSize = 4096;
char buffer[4096];

bool writeData(S3tpChannel& channel, std::string line) {
    struct horst::s3tp_horst_header header;
    size_t toWrite = 0;
    size_t i = 0;

    header.length = line.length();
    header.complete = true;
    header.type = horst::s3tp_horst_type::SIMPLE;

    // Send length of msg
    if (channel.send(&header, sizeof(header)) <= 0) {
        return false;
    }

    // Send command
    const char * data = line.data();
    while (i < header.length) {
        toWrite = (size_t) std::min(bufferSize, header.length);
        memcpy(buffer + i, data, toWrite);
        if (channel.send(buffer, toWrite) <= 0) {
            return false;
        }
        i += toWrite;
    }

    return true;
}

char * readData(S3tpChannel& channel, size_t& len) {
    char * readBuffer;
    horst::s3tp_horst_header header;

    if (channel.recv(&header, sizeof(header)) <= 0) {
        return nullptr;
    }
    readBuffer = new char[header.length + 1];
    if (channel.recv(readBuffer, header.length) <= 0) {
        delete [] readBuffer;
        return nullptr;
    }
    readBuffer[header.length] = '\0';
    len = header.length;

    return readBuffer;
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "Usage: remoteexec <your command>" << std::endl;
        return 0;
    }

    std::string command = "";
    for (int i = 1; i < argc; i++) {
	command += argv[i];
    }

    s3tpSocketPath = SOCKETPATH;
    int error = 0;
    ClientConfig config;
    config.port = (uint8_t) PORT_LOCAL;
    config.channel = 3;
    config.options = S3TP_OPTION_ARQ;

    // Synchronous mode
    S3tpChannelSync channel(config);
    channel.bind(error);
    if (error != 0) {
	std::cerr << "Couldn't bind to port " << std::to_string(PORT_LOCAL)
	    << ", due to error " << std::to_string(error) << std::endl;
        return 1;
    }
    sleep(1);

    // Connect
    if (channel.connect(PORT_HORST) < 0) {
	std::cerr << "Could not connect to HORST on port " << std::to_string(PORT_HORST) << std::endl;
        return 1;
    }
    std::cout << "Connection to other endpoint established" << std::endl;

    // Write command
    if (!writeData(channel, command + "\n")) {
	std::cerr << "An error occurred while sending data over the channel. Quitting" << std::endl;
	return 1;
    }
    std::cout << "Payload sent. Waiting for reply..." << std::endl;

    for (int i = 0; i <= 1; i++) {
	size_t len = 0;
	char* rcvData = readData(channel, len);
	if (rcvData == nullptr) {
	    std::cout << "An error occurred while reading data from the channel. Quitting" << std::endl;
	    return 1;
	}
	std::cout << "Received message: " << std::string(rcvData) << std::endl;
	delete [] rcvData;
    }

    if (channel.isConnected()) {
	channel.disconnect();
    }
    std::cout << "Quitting..." << std::endl;

    return 0;
}
