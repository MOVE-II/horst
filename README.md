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


Algorithm
---------

HORST works the following way:

``` python

current_state = State()


while (True):
    # sleep until some change arrives
    event = event_loop_wait(s3tp, dbus, ...)

    # update the current state with a fact
    if event.is_fact():
        event.update(current_state)

    # or update the target state with a request
    target_state = current_state.copy()
    if not event.is_fact():
        event.update(target_state)

    # determine actions to perform when
    # we change from current to target state
    actions = current_state.transform_to(target_state)

    # enqueue those actions in the event loop
    event_loop_enqueue(actions)
```

The `current_state.transform_to` method does the actual work.
It determines what is necessary to come from `current_state`
to `target_state` by sending out requests. When requests are sent,
the `current_state` is updated to store the "request in progress".

The transformation is done with a table.
This table enforces constraints on the state.

If some new state is requested, the `target_state` is just set to the
desired outcome, but the transformation via this table is done afterwards.
This means for example if the power is not ok, no matter the request,
the transmitter will stay off.


| Temp ok | Power ok | transmit | mode | -> transmit | -> mode  |
|---------|----------|----------|------|-------------|----------|
| X       | 0        | _        | _    |           0 | fallback |
| 0       | X        | _        | _    |           0 | fallback |
| X       | X        | 1        | _    |           _ | speed    |


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
