#!/bin/bash

(
  repeat 3 1 busctl --system call moveii.adcs /moveii/adcs moveii.adcs setMode s SUN
) &> /dev/null
