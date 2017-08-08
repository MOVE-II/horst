#!/bin/bash

# This small wrapper for the remoteexec client will make sure the
# s3tp socket file is writable for us and eliminate all debug output
# so the user only has the pure command output as result.

# Make socket writable, if not done yet
if [ ! -w /tmp/s3tp.a ]; then
	>&2 echo "S3TP socket is not writable. Changing file permissions..."
	sudo chmod 777 /tmp/s3tp.a
fi

/home/ops/dependencies/cdh_horst/test/s3tp/build/remoteexec $@ 2> /dev/null
