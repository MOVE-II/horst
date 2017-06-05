Horst
=====

Humble On-board Reconfiguration State Transformer

Description
-----------

HORST is the main management software entity on the MOVE-II board computer.

HORST is responsible for keeping the satellite in a safe state by reacting on
critical events like low battery or high temperatures with appropriate actions.
It maintains a global view of the state of the satellite and can check
cross-subsystem dependencies.

It listens on state signals of other daemons on D-Bus and stores the current
state in his internal memory and thereby gains a global view on the state
of the satellite.
Based on this global state view HORST can decide to react on defined conditions
with also predefined actions.
A logic state table is therefore processed on each incoming state update event
what might invoke actions.
These actions are provided as shell scripts.

Besides the D-Bus interface for satellite local commanding HORST also implements
an S3TP interface for executing arbitrary shell commands on the satellite.

Installation
------------

Have a C++14 compiler and CMake.

```sh
mkdir build && cd build && cmake .. && make -j4
```

Dependencies
------------

Depends on systemd for connecting to D-Bus, libuv for the event loop,
systemd++ for a C++ systemd interface and S3TP for communication with ground
station.

Artifacts
---------

- build/horst
- root/opt/cdh\_horst/scripts

Usage
-----

Just run the daemon without parameters.

```sh
./horst
```

Further documentation
---------------------

* [Operation Manual](./doc/operation_manual.md)
* [System Design](./doc/system_design.md)
* [Diagram](./doc/sequence.svg)
* Doxygen Documentation (tbd)

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
