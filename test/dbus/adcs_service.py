#!/usr/bin/env python

import dbus
import dbus.service
import dbus.mainloop.glib

import gobject


class Service(dbus.service.Object):
    def __init__(self, message):
        self._loop = gobject.MainLoop()
        self._message = message

    def run(self):
        dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
        bus_name = dbus.service.BusName("moveii.adcs", dbus.SessionBus())
        dbus.service.Object.__init__(self, bus_name, "/moveii/adcs")
        print "Service running..."
        self._loop.run()
        print "Service stopped"

    @dbus.service.method("moveii.adcs", in_signature='s', out_signature='b')
    def setMode(self, arg):
        print "Received " + str(arg)
        self.append_result(str(arg))
        return True

    def append_result(self, state):
        with open("adcs_result.txt", "w") as result_file:
            result_file.write(state)


if __name__ == "__main__":
    Service("ADCS mock service").run()
