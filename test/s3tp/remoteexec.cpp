#include <s3tp/connector/S3tpChannelSync.h>
#include <fstream>


const std::string SUB_COMPONENT = "S3TP";
const bool TIMESTAMP_ENABLED = false;

const uint8_t PORT_HORST = 99;
const uint8_t PORT_LOCAL = 17;
const char* SOCKETPATH = "/tmp/s3tp.a";


const uint32_t bufferSize = 4096;
char buffer[4096];

bool writeData(S3tpChannel& channel, std::string line) {
    size_t toWrite = 0;
    size_t i = 0;
    uint32_t len = line.length();

    // Send length of msg
    if (channel.send(&len, sizeof(len)) <= 0) {
        return false;
    }

    // Send command
    const char * data = line.data();
    while (i < len) {
        toWrite = (size_t) std::min(bufferSize, len);
        memcpy(buffer + i, data, toWrite);
        if (channel.send(buffer, toWrite) <= 0) {
            return false;
        }
        i += toWrite;
    }

    return true;
}

char * readData(S3tpChannel& channel, uint32_t& len) {
    char * readBuffer;
    if (channel.recv(&len, sizeof(len)) <= 0) {
	return nullptr;
    }
    readBuffer = new char[len + 1];
    if (channel.recv(readBuffer, len) <= 0) {
        delete [] readBuffer;
        return nullptr;
    }
    readBuffer[len] = '\0';

    return readBuffer;
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "Usage: remoteexec <your command>" << std::endl;
        return 0;
    }

    std::string command = "";
    for (int i = 1; i < argc; i++) {
	if (i > 1)
		command += " ";
	command += argv[i];
    }

    s3tpSocketPath = SOCKETPATH;
    int error = 0;
    ClientConfig config;
    config.port = (uint8_t) PORT_LOCAL;
    config.channel = 3;
    config.options = S3TP_OPTION_ARQ;

    // Synchronous mode
    LOG_DEBUG("Binding...");
    S3tpChannelSync channel(config);
    channel.bind(error);
    if (error != 0) {
	LOG_ERROR("Couldn't bind to port " + std::to_string(PORT_LOCAL) + " due to error " + std::to_string(error));
	return 1;
    }
    sleep(1);

    // Connect
    LOG_DEBUG("Connecting...");
    if (channel.connect(PORT_HORST) < 0) {
	LOG_ERROR("Could not connect to HORST on port " + std::to_string(PORT_HORST));
	return 1;
    }
    LOG_INFO("Connection to HORST established.");

    // Write command
    if (!writeData(channel, command + "\n")) {
	LOG_ERROR("An error occurred while sending data over the channel. Quitting.");
	return 1;
    }
    LOG_DEBUG("Payload sent. Waiting for reply....");

    while (true) {
	uint32_t len = 0;
	char* rcvData = readData(channel, len);
	if (rcvData == nullptr) {
	    LOG_ERROR("Error occurred while reading data from the channel. Quitting.");
	    return 1;
	}
	if (len == 3 && strncmp(rcvData, "ack", 3) == 0) {
		LOG_INFO("HORST has received the command.");
		delete [] rcvData;
		continue;
	}
	if (strncmp(rcvData, "[exit] ", 6) == 0) {
		LOG_INFO("Command completed with exit status: " + std::string(rcvData));
		delete [] rcvData;
		break;
	}
	std::cout << std::string(rcvData);
	delete [] rcvData;
    }

    if (channel.isConnected()) {
	channel.disconnect();
    }
    LOG_DEBUG("Quitting.");

    return 0;
}
