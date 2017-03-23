#!/bin/bash

(
	systemctl start pl.service
	busctl --system call moveii.eps /moveii/eps moveii.eps switchOn s PLTHM

	busctl --system call moveii.eps /moveii/eps moveii.eps switchOn s ADCS5V
	busctl --system call moveii.eps /moveii/eps moveii.eps switchOn s ADCS3V3
	sleep 10
	busctl --system call moveii.adcs /moveii/adcs moveii.adcs setMode s DETUMB
) &> /dev/null

