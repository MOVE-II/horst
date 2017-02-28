#!/usr/bin/env python

import dbus


class Client(object):
    def __init__(self):
        bus = dbus.SessionBus()
        service = bus.get_object('moveii.horst', "/moveii/horst")
        self._message = service.get_dbus_method('getBeaconData', 'moveii.horst')

    def run(self):
        return self._message()

if __name__ == "__main__":
    values = Client().run()
    for item in values:
        print int(item)
