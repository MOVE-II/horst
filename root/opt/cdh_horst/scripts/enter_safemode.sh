#!/bin/bash

# ADCS off, PL off, GPS off, S-Band off

(
  REPEATS=3
  SHORTWAIT=1
  LONGWAIT=2

	documentSafemode

	systemctl stop pl.service
	sleep $LONGWAIT
	repeat $REPEATS $SHORTWAIT busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s PLTHM

	repeat $REPEATS $SHORTWAIT busctl --system call moveii.adcs /moveii/adcs moveii.adcs setMode s POWEROFF
	sleep $LONGWAIT
	repeat $REPEATS $SHORTWAIT busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s ADCS5V
	repeat $REPEATS $SHORTWAIT busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s ADCS3V3

	repeat $REPEATS $SHORTWAIT busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s SBAND

	systemctl stop gps_active.service

	script_full_path=$(dirname "$0")
	if /bin/bash "$script_full_path/check_leop.sh"; then
		# LEOP is done

		systemctl start sleepwake.service
	fi


) &> /dev/null
