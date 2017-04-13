#!/bin/bash

(
  script_full_path=$(dirname "$0")
  if /bin/bash "$script_full_path/check_leop.sh"; then
    # LEOP is done

    # Switch on PL
    repeat 3 2 busctl --system call moveii.eps /moveii/eps moveii.eps switchOn s PLTHM
  fi

  # All this lines will be executed on startup of horst
  busctl --system call moveii.adcs /moveii/adcs moveii.adcs setMode s SLEEP
) &> /dev/null
