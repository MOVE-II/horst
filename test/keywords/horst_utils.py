def horst_beacon_data(raw_beacon):
    beacon_fields = ['safemode', 'manualmode',
                     'battery_msb', 'battery_lsb',
                     'thermal', 'adcs_pointing',
                     'adcs_requested', 'payload', 'LEOP']
    if len(beacon_fields) != len(raw_beacon):
        raise AttributeError("Raw beacon data too short beacon[{0}]".format(",".join(raw_beacon)))
    return dict(zip(beacon_fields, raw_beacon))


def integer_from_bytes(msb, lsb):
    return int(msb) << 8 ^ int(lsb)


def get_adcs_states():
    return {'0': 'SLEEP',
            '2': 'ATTDET',
            '4': 'DETUMB',
            '7': 'SUN',
            '8': 'NADIR',
            '11': 'EXP',
            '13': 'FLASH'}


def leop_state_id(state_name):
    states = {'UNDEPLOYED': 1, 'DEPLOYED': 2, 'DONE': 3}[state_name]
    if state_name not in states:
        raise AttributeError("Invalid LEOP state<{0}>".format(state_name))
    return states[state_name]


def adcs_state_as_string(state):
    states = {'0': 'SLEEP',
              '2': 'ATTDET',
              '4': 'DETUMB',
              '7': 'SUN',
              '8': 'NADIR',
              '11': 'EXP',
              '13': 'FLASH'}
    if state not in states:
        raise AttributeError("Invalid ADCS state id<{0}>".format(state))
    return get_adcs_states()[state]


def get_thermal_state_as_string(state_id):
    states = {'0': "OK", '1': "WARN", '2': "ALARM"}
    if state_id not in states:
        raise AttributeError("Invalid thermal state id<{0}>".format(state_id))
    return states[state_id]
