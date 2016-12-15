#pragma once

#include <functional>
#include <memory>
#include <string>

#include "event.h"


namespace horst {

/**
 * Control messages via s3tp from earth.
 */
class ControlMessage : public Event {
public:
	/**
	 * Used to provide the result of the control message to earth.
	 */
	using done_cb_t = std::function<void(const std::string &)>;

	ControlMessage();
	virtual ~ControlMessage() = default;

	/**
	 * Parse the given command and return the appropriate
	 * control message object.
	 */
	static std::shared_ptr<ControlMessage> parse(const std::string &msg);

	/**
	 * control message is never a fact.
	 * @returns false
	 */
	bool is_fact() const override;

	/**
	 * install a new callback that is fired when the control message was handled.
	 */
	void call_on_complete(done_cb_t on_done);

	/**
	 * Call this function if you completed the request of the control message.
	 * This will notify that the message was handled.
	 */
	void done(const std::string &result);

protected:
	/** callback that is triggered with the done() call above. */
	done_cb_t on_done;
};

} // horst
