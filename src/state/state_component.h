#pragma once

namespace horst {

/**
 * Something that makes up the state of the satellite.
 */
class StateComponent {
public:
	StateComponent();
	virtual ~StateComponent() = default;
};

} // horst
