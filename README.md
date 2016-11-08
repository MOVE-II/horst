Horst
=====

Humble On-board Reconfiguration State Transformer


HORST is the main management software entity on the MOVE-II board computer.

It synchronizes the states of all the subsystem daemons in order to bring the
satellite into cross-subsystem states. This allows to check for state
dependencies between subsystems.

HORST sends requests to a subsystem daemon via D-Bus. The daemon has to react on
that request, run code to fulfill it and confirms actions with another D-Bus
message. That way, HORST instructs subsystems to perform actions and in
return gets feedback about operations that were performed.

Subsystems must answer to requests by HORST. They can listen for D-Bus events of
other subsystems as well and react to those additionally. This ensures that
subsystem may perform communication irrelevant for HORST via D-Bus.

For example, to activate S-Band transmission, the EPS has to provide enough
power resources and OMAC is required to point the antenna towards the ground
station. HORST's purpose is it to synchronize this activation procedure.


Building
--------

Have a C++14 compiler and CMake.

`mkdir build && cd build && cmake .. && make -j4`


License
-------


Released under the GNU GPLv3 or later.
See [COPYING](COPYING) for further info.

    HORST
    Copyright (C) 2016 Jonas Jelten <jelten@in.tum.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
