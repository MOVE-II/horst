#!/bin/bash

(
	busctl --system call moveii.eps /moveii/eps moveii.eps switchOn s PLTHM
) &> /dev/null

