# ADCS off, PL off, GPS off, S-Band off, GPS off

(
	systemctl stop pl.service
	systemctl stop gps.service

	sleep 10
  
	busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s PLTHM

	busctl --system call moveii.adcs /moveii/adcs moveii.adcs setMode s ATTDET
	busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s ADCS5V
	busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s ADCS3V3

	busctl --system call moveii.eps /moveii/eps moveii.eps switchOff s SBAND
) &> /dev/null
