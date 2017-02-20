#include "thm.h"


namespace horst {

THM::THM()
	:
	all_temp{overall_temp::OK} {}

std::vector<std::unique_ptr<Action>>
THM::transform_to(const THM & /*target*/) const {

	std::vector<std::unique_ptr<Action>> ret;


	return ret;
}

/*
/moveii/thm
moveii.thm
char getSystemState()
  ->
    #include "thm_externals.h"
    enum thm::SystemState {
        OK,       // temp ok
        WARNING,  // should turn off something
        ALARM     // have to turn off something
    }
*/

} // horst
