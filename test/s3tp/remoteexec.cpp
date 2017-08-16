#include <condition_variable>
#include <fstream>
#include <mutex>
#include <s3tp/connector/S3tpCallback.h>
#include <s3tp/connector/S3tpChannelAsync.h>

#include "../../src/server/s3tp_proto.h"


const std::string SUB_COMPONENT = "S3TP";
const bool TIMESTAMP_ENABLED = false;

const uint8_t PORT_HORST = 99;
const uint8_t PORT_LOCAL = 17;
const char* SOCKETPATH = "/tmp/s3tp.a";

bool volatile running = true;
std::mutex asyncMutex;
std::condition_variable asyncCond;

class RemoteexecCallback : public S3tpCallback {

    /**
     * Buffer for received data
     */
    std::vector<char> buf;

    /**
     * Number of bytes expected for command
     */
    uint32_t expected;

    /**
     * Received types
     */
    MessageType type;

    /**
     * Handle incoming data
     */
    void onDataReceived(S3tpChannel &channel, char *data, size_t len) override {
	const size_t headersize = sizeof(this->expected) + sizeof(this->type);

	// Copy data into buffer
	this->buf.insert(this->buf.end(), data, data + len);

	// Not enough data yet
	if (this->buf.size() < headersize) {
		LOG_DEBUG("[s3tp] Not enough data received, waiting for more...");
		return;
	}

	// Receive header
	if (this->expected == 0) {
		LOG_DEBUG("[s3tp] Receiving new command...");
		std::memcpy(&this->expected, this->buf.data(), sizeof(this->expected));
		std::memcpy(&this->type, this->buf.data() + sizeof(this->expected), sizeof(this->type));
		if (this->expected == 0 && type == MessageType::NONE) {
		    throw std::runtime_error("Invalid length received!");
		    return;
		}
	}

	// Wait until all data of packet are received
	if (this->buf.size() < this->expected + headersize) {
		return;
	}

	std::string indata(this->buf.begin()+headersize, this->buf.begin()+headersize+this->expected);

	if ((uint8_t) this->type & (uint8_t) MessageType::STARTED) {
	    LOG_INFO("HORST has received the command.");
	} else if ((uint8_t) this->type &  (uint8_t) MessageType::ENDOFFILE) {
	    LOG_INFO("Command completed with exit status: " + std::string(indata));
	    running = false;
	} else {
	    std::cout << indata;
	}

	this->buf.erase(this->buf.begin(), this->buf.begin() + headersize + this->expected);
	this->expected = 0;
    }

    void onConnected(S3tpChannel &channel) override {
	LOG_INFO("Connection to HORST established successfully.");
	asyncCond.notify_all();
    }
    void onDisconnected(S3tpChannel &channel, int error) override {
	LOG_INFO("HORST closed the connection.");
	running = false;
    }
    void onBufferFull(S3tpChannel& channel) override {}
    virtual void onBufferEmpty(S3tpChannel& channel) override {}
    void onError(int error) override {}
};

bool writeData(S3tpChannel& channel, std::string line, MessageType type) {
	int r;
    uint32_t len = line.length();
    std::vector<char> sendbuf;

    // Prepare send buffer
    sendbuf.resize(sizeof(len) + sizeof(type));
    std::memcpy(sendbuf.data(), &len, sizeof(len));
    std::memcpy(sendbuf.data() + sizeof(len), &type, sizeof(type));
    sendbuf.insert(sendbuf.end(), &line[0], &line[0] + len);

    // Send message in buffer
    r = channel.send(sendbuf.data(), sendbuf.size());
    if (r <= 0) {
	LOG_ERROR("Error " + std::to_string(r) + " occurred while sending data over the channel. Quitting.");
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
	std::cout << "Usage: remoteexec <your command>" << std::endl;
	return 0;
    }

    std::unique_lock<std::mutex> lock{asyncMutex};

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

    // Asynchronous mode
    LOG_DEBUG("Binding...");
    RemoteexecCallback cb;
    S3tpChannelAsync channel(config, cb);
    channel.bind(error);
    if (error != 0) {
	LOG_ERROR("Couldn't bind to port " + std::to_string(PORT_LOCAL) + " due to error " + std::to_string(error));
	return 1;
    }

    // Connect
    LOG_DEBUG("Connecting...");
    if (channel.connect(PORT_HORST) < 0) {
	LOG_ERROR("Could not connect to HORST on port " + std::to_string(PORT_HORST));
	return 1;
    }
    asyncCond.wait(lock);

    // Write command
    if (!writeData(channel, command + "\n", MessageType::NONE)) {
	LOG_ERROR("An error occurred while sending data over the channel. Quitting.");
	return 1;
    }
    LOG_DEBUG("Payload sent. Waiting for reply....");

    // Wait for input on stdin and send to HORST
    bool has_in = true;
    while (running) {
		struct timeval tv;
		fd_set fds;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		if (has_in)
			FD_SET(STDIN_FILENO, &fds);
		int ret = select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
		if (ret < 0) {
		    throw std::runtime_error("Failed to read from stdin!");
		}
		if (FD_ISSET(STDIN_FILENO, &fds)) {
			command.resize(1<<16);
			int n = read(0, (char*)command.data(), command.size());
			if (n < 0) {
				LOG_ERROR("An error occurred while reading data from stdin. Quitting.");
				return 1;
			}
			command.resize(n);
			if (command.empty()) {
				LOG_INFO("EOF");
				command = "";
				has_in = false;
				if (!writeData(channel, "", MessageType::ENDOFFILE))
				    return 1;
			}
			if (command.size())
			    if (!writeData(channel, command, MessageType::STDIN))
				return 1;
		}
    }
    LOG_INFO("Quitting.");

    if (channel.isConnected()) {
	channel.disconnect();
    }

    return 0;
}
