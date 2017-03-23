#include "finish_leop.h"

#include <sstream>

#include "../event/leop_signal.h"
#include "../satellite.h"


namespace horst {

FinishLEOP::FinishLEOP()
    : ShellCommand("finish_leop.sh", nullptr) {}


std::string FinishLEOP::describe() const {
	std::stringstream ss;
	ss << "Finish LEOP";
	return ss.str();
}


void FinishLEOP::perform(Satellite *sat, ac_done_cb_t done) {
	LOG_INFO("[action] Finishing LEOP");
	sat->on_event(std::make_shared<LEOPSignal>(State::leop_seq::DONE));
	ShellCommand::perform(sat, [sat, done] (bool success, Action *action) {
		LOG_INFO("[action] LEOP is done now");
		done(success, action);
	});
}

} // horst
