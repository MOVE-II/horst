import dbus


class MoveiiClient(object):
    def __init__(self, daemon):
        bus = dbus.SessionBus()
        self._daemon = daemon
        self._service = bus.get_object('moveii.' + daemon, "/moveii/" + daemon)

    def run_method(self, method_name, signature='', args=None):
        if args is None:
            args = []
        return self._service.get_dbus_method(method_name, 'moveii.' + self._daemon, args, signature=signature)()


def beacon_data_as_list(beacon_string):
    return beacon_string.split(' ')[2::]
