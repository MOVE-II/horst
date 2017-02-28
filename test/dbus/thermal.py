#!/usr/bin/env python

import dbus
import dbus.service
import dbus.mainloop.glib
import sys


class ThermalService(dbus.service.Object):
    def __init__(self, connection):
        dbus.service.Object.__init__(self, connection, "/moveii/thm")

    @dbus.service.signal(dbus_interface='moveii.thm', signature='s')
    def thmStateChange(self, state):
        print state


if __name__ == "__main__":
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    session_bus = dbus.SessionBus()
    name = dbus.service.BusName('moveii.thm', session_bus)
    thermal = ThermalService(session_bus)
    thermal.thmStateChange(sys.argv[1])
