#include "trigger_sunpointing.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "../satellite.h"


namespace horst {

TriggerSunpointing::TriggerSunpointing()
    : ShellCommand("./scripts/trigger_sunpointing.sh", nullptr) {}


std::string TriggerSunpointing::describe() const {
	std::stringstream ss;
	ss << "Trigger sunpointing on adcs daemon";
	return ss.str();
}


void TriggerSunpointing::perform(Satellite *sat, ac_done_cb_t done) {
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		if (success) {
			std::cout << "Sunpointing has been triggered." << std::endl;
			// TODO: Create adcs state change signal for sunpointing requested
			//sat->on_event(std::make_shared<ADCSSignal>(sth));
		} else {
			// Just ignore, pl daemon will ask again soon
		}
		done(success, action);
	});
}

} // horst
