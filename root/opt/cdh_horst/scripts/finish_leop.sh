#!/bin/bash

(
  # This is necessary as the service is already started by the SCIOPS.target,
  # but the power is not turned on yet.

  repeat 3 2 busctl --system call moveii.eps /moveii/eps moveii.eps switchOn s PLTHM
) &> /dev/null
