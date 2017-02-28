#!/usr/bin/env python

import dbus
import dbus.service
import dbus.mainloop.glib
import sys


class EPSService(dbus.service.Object):
    def __init__(self, connection):
        dbus.service.Object.__init__(self, connection, "/moveii/eps")

    @dbus.service.signal(dbus_interface='moveii.eps', signature='q')
    def epsChargeStateChange(self, state):
        print state


if __name__ == "__main__":
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    session_bus = dbus.SessionBus()
    name = dbus.service.BusName('moveii.eps', session_bus)
    eps = EPSService(session_bus)
    eps.epsChargeStateChange(int(sys.argv[1]))
