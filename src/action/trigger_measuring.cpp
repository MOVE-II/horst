#include "trigger_measuring.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../logger.h"
#include "../satellite.h"


namespace horst {

TriggerMeasuring::TriggerMeasuring()
    : ShellCommand("trigger_measuring.sh", nullptr) {}


std::string TriggerMeasuring::describe() const {
	std::stringstream ss;
	ss << "Trigger measuring on payload daemon";
	return ss.str();
}


void TriggerMeasuring::perform(Satellite *sat, ac_done_cb_t done) {
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		if (success) {
			LOG_INFO("[action] Measuring has been triggered");
			// TODO: Create pl state change signal for measuring
			//sat->on_event(std::make_shared<PayloadSignal>(sth));
		} else {
			// Just ignore, pl daemon will ask again soon
		}
		done(success, action);
	});
}

} // horst
