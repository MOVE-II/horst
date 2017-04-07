#!/bin/bash

(
	repeat 3 2 busctl --system call moveii.eps /moveii/eps moveii.eps switchOn s PLTHM
) &> /dev/null
