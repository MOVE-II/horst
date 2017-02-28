#!/usr/bin/env python

import dbus
import dbus.service
import dbus.mainloop.glib
import sys


class AdcsService(dbus.service.Object):
    def __init__(self, connection):
        dbus.service.Object.__init__(self, connection, "/moveii/adcs")

    @dbus.service.signal(dbus_interface='moveii.adcs', signature='s')
    def adcsStateReached(self, state):
        print state


if __name__ == "__main__":
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    session_bus = dbus.SessionBus()
    name = dbus.service.BusName('moveii.adcs', session_bus)
    adcs = AdcsService(session_bus)
    adcs.adcsStateReached(sys.argv[1])
