#include "trigger_detumbling.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../satellite.h"


namespace horst {

TriggerDetumbling::TriggerDetumbling()
    : ShellCommand("./scripts/trigger_detumbling.sh", nullptr) {}


std::string TriggerDetumbling::describe() const {
	std::stringstream ss;
	ss << "Trigger detumbling on adcs daemon";
	return ss.str();
}


void TriggerDetumbling::perform(Satellite *sat, ac_done_cb_t done) {
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		if (success) {
			std::cout << "Detumbling has been triggered." << std::endl;
			// TODO: Create adcs state change signal for detumbling requested
			//sat->on_event(std::make_shared<ADCSSignal>(sth));
		} else {
			// Just ignore, pl daemon will ask again soon
		}
		done(success, action);
	});
}

} // horst
