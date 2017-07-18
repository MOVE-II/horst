#!/bin/bash

(
  REPEATS=3
  SHORTWAIT=1

  script_full_path=$(dirname "$0")
  if /bin/bash "$script_full_path/check_leop.sh"; then
    # LEOP is done

    # Switch on PL
    repeat $REPEATS $SHORTWAIT busctl --system call moveii.eps /moveii/eps moveii.eps switchOn s PLTHM

    # This is automatically done by the SCIOPS.target:
    # systemctl start pl.service
  fi

  # All these lines will be executed on startup of horst
  repeat $REPEATS $SHORTWAIT busctl --system call moveii.eps /moveii/eps moveii.eps switchOn s ADCS3V3
  repeat $REPEATS $SHORTWAIT busctl --system call moveii.eps /moveii/eps moveii.eps switchOn s ADCS5V
) &> /dev/null
