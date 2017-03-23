#!/bin/bash

(
	script_full_path=$(dirname "$0")
	if /bin/bash "$script_full_path/checkleop.sh"; then
		# LEOP is done

		# Switch on PL
		busctl --system call moveii.eps /moveii/eps moveii.eps switchOn s PLTHM
	fi

	# All this lines will be executed on startup of horst
	busctl call moveii.adcs /moveii/adcs moveii.adcs setMode s SLEEP
) &> /dev/null

