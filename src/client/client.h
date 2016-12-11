#pragma once

#include <memory>

namespace horst {

class Satellite;

/**
 * Some client to control horst.
 * It generates events.
 */
class Client {
	static constexpr size_t max_buf_size = 4096;

public:
	Client(Satellite *satellite);

	Client(Client &&other) = delete;
	Client &operator =(Client &&other) = delete;
	Client(const Client &other) = delete;
	Client &operator =(const Client &other) = delete;

	virtual ~Client() = default;

	/** called when this client receives data */
	void data_received(const char *data, size_t size);

	/** call this to send data to the client */
	virtual void send(const char *buf, size_t len) = 0;

	/**
	 * call this to send c-strings.
	 * internally calls this->send(text, strlen(text))
	 */
	void send(const char *text);

	/** close the connection */
	virtual void close() = 0;

protected:
	/** Satellite the client connected to */
	Satellite *satellite;

	/** buffer for input commands */
	std::unique_ptr<char[]> buf;

	/** position in the read buffer */
	size_t buf_used;
};

} // horst
