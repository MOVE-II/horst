# HORST
<!-- Template version: 1.0 -->


## Description

HORST is the main management software entity on the MOVE-II board computer.

HORST is responsible for keeping the satellite in a safe state by reacting on
critical events like low battery or high temperatures with appropriate actions.

HORST listens on state signals of other daemons on D-Bus and stores the current
state in his internal memory and thereby gains a global view on the state
of the satellite, which allows a check of cross-subsystem dependencies.
Based on this global state view HORST can decide to react on defined conditions
with also predefined actions.
A logic state table is processed on each incoming state update event
what might invoke actions.
These actions are provided as shell scripts.

Besides the D-Bus interface for satellite local commanding HORST also implements
an S3TP interface for executing arbitrary shell commands on the satellite.


## Dependencies

* D-Bus
* systemd
* systemd++
* libuv
* libdl
* trctrl
* s3tp

More details can be found in the [CMake file](CMakeLists.txt)


## Artifacts

- build/horst
- test/s3tp/build/remoteexec
- root/etc/dbus-1/system.d/cdh\_horst.conf
- root/lib/systemd/system/horst.service
- root/lib/systemd/system/horst\_deployed.timer
- root/lib/systemd/system/horst\_deployed.service
- root/opt/cdh\_horst/scripts/check\_leop.sh
- root/opt/cdh\_horst/scripts/trigger\_detumbling.sh
- root/opt/cdh\_horst/scripts/leave\_safemode.sh
- root/opt/cdh\_horst/scripts/enter\_manualmode.sh
- root/opt/cdh\_horst/scripts/leave\_manualmode.sh
- root/opt/cdh\_horst/scripts/enter\_safemode.sh
- root/opt/cdh\_horst/scripts/trigger\_sunpointing.sh
- root/opt/cdh\_horst/scripts/startup.sh
- root/opt/cdh\_horst/scripts/finish\_leop.sh
- root/opt/cdh\_horst/scripts/trigger\_measuring.sh

More details can be found in the [CMake file](CMakeLists.txt)


## Licensing

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


## Further Documentation

* General Documentation
  * [Operation Manual](./doc/operation_manual.md)
  * [Interfaces](./doc/interfaces.md)
  * Further parts
    <!-- REPLACE WITH:
        Extend this list or remove the whole point if not applicable
    -->
    * [comraw] README
    * [linkfire] README
* Technical Documentation
  * [System Design](./doc/system_design.md)
  * [Sequence Diagram](./doc/sequence.svg)
  * [Architecture Diagram](./doc/architecture.svg)
  * [Doxygen Documentation](https://docs.move2space.de)
* [MOVE-II System Documentation](https://gitlab.lrz.de/move-ii/move-ii_system-documentation)
* [MOVE-II Operation Manual](#)
