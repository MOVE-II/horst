#!/bin/bash

dbus-send --system --type=signal /moveii/pl moveii.pl.doMeasurement &> /dev/null
