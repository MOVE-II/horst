#!/bin/bash

# ADCS off, PL off, GPS off, S-Band off

(

	systemctl stop pl.service
	sleep 5
	repeat 3 2 busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s PLTHM

	sleep 5

	busctl --system call moveii.adcs /moveii/adcs moveii.adcs setMode s SLEEP
	sleep 5
	repeat 3 2 busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s ADCS5V
	repeat 3 2 busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s ADCS3V3

	sleep 5

	repeat 3 2 busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s SBAND

	systemctl stop gps_active.service

) &> /dev/null
