#!/bin/bash

# Return 0, if leop is done, 1 if not
systemctl list-units --type target | grep -i SCIOPS | grep active &> /dev/null
