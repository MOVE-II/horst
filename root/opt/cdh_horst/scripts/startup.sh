#!/bin/bash

(
  # Put startup commands here, if you need any.
  # Initial state is set in startup_service.sh as separate systemd oneshot service
  exit 0
) &> /dev/null
