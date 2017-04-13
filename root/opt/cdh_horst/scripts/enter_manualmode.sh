#!/bin/bash

(
	# Disable manualmode automatically after 30min
	sleep 1800

	busctl --system call moveii.horst /moveii/horst moveii.horst setManualmode s FALSE
) &> /dev/null
