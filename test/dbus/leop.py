#!/usr/bin/env python

import dbus
import dbus.service
import dbus.mainloop.glib
import sys


class LeopService(dbus.service.Object):
    def __init__(self, connection):
        dbus.service.Object.__init__(self, connection, "/moveii/leop")

    @dbus.service.signal(dbus_interface='moveii.leop', signature='y')
    def leopStateChange(self, state):
        print state


if __name__ == "__main__":
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    session_bus = dbus.SessionBus()
    name = dbus.service.BusName('moveii.leop', session_bus)
    leop = LeopService(session_bus)
    leop.leopStateChange(int(sys.argv[1]))
