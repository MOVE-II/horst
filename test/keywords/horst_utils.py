def horst_beacon_data(raw_beacon):
    beacon_fields = ['safemode', 'manualmode',
                     'battery', 'battery',
                     'thermal', 'adcs_pointing',
                     'adcs_requested', 'payload', 'LEOP']
    if len(beacon_fields) != len(raw_beacon):
        raise AttributeError("Raw beacon data to short beacon[{0}]".format(",".join(raw_beacon)))
    return dict(zip(beacon_fields, raw_beacon))
