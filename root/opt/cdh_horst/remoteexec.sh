#!/bin/bash

# This small wrapper for the remoteexec client will make sure the
# s3tp socket file is writable for us and eliminate all debug output
# so the user only has the pure command output as result.

LOGFILE=/opt/cdh_horst/remoteexec.log

# Make socket writable, if not done yet
if [ ! -w /tmp/s3tp.a ]; then
	>&2 echo "S3TP socket is not writable. Changing file permissions..."
	sudo chmod 777 /tmp/s3tp.a
fi

# Create log file
if [ ! -w "$LOGFILE" ]; then
	>&2 echo "Log file is not writable. Changing file permissions..."
	if [ ! -f "$LOGFILE" ]; then
		sudo touch "$LOGFILE"
	fi
	sudo chmod 777 "$LOGFILE"
fi

/opt/cdh_horst/remoteexec $@ 2>> "$LOGFILE"
