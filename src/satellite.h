#pragma once


#include "horst.h"

#include "daemon.h"


namespace horst {

class Satellite {
public:
	Satellite(const arguments &args);
	virtual ~Satellite() = default;

	/**
	 * Launch the satellite state processing.
	 */
	void loop();

protected:

private:
	const arguments &args;

	Daemon com;
	Daemon payload;
};

}
