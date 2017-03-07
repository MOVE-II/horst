#!/bin/bash

dbus-send --system --dest=moveii.pl --type=signal /moveii/pl moveii.pl.doMeasurement &> /dev/null

