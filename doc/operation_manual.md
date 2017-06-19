Error Codes
===========

|   Code   |      Text      |  Description  | Next steps |
|----------|----------------|---------------|------------|
| HORST-1 | Internal error! | An internal error occurred and the daemon has been stopped | Examine error message, fix problem if possible and restart horst |
| HORST-2 | Fatal C++ error! | An unhandled exception was thrown and the daemon has been stopped | Examine error message, fix problem if possible and restart horst |
| HORST-3 | function() failed to parse parameters | Parameters to this function could not be read from D-Bus, call is ignored | Fix D-Bus call to provide the correct parameters, make sure D-Bus works correctly, retry sending call |
| HORST-4 | Failed to connect to bus | Daemon was not able to connect to D-Bus | Make sure D-Bus is available and restart daemon |
| HORST-5 | Failed to acquire service name | Daemon was not able to acquire his service name on D-Bus | Make sure horst is only running once, there is no other service with the same D-Bus service name and restart daemon |
| HORST-6 | Failed to add xyz match | Daemon was not able to register for named signal and will not receive updates for it | Make sure D-Bus is working properly and restart daemon |
| HORST-7 | Failed to install the horst sdbus object | Daemon was not able to fully initialize his D-Bus functionality | Make sure D-Bus is working and restart daemon |
| HORST-8 | Failed to check LEOP status | The check\_leop.sh script returned something different than 0 or 1 | Check script and restart daemon |
| HORST-9 | Failed to listen on DBus| A connection to DBus could not be established | Make sure DBus is available and restart daemon |
| HORST-10 | Failed to listen on S3TP | A connection to S3TP could not be established | Make sure S3TP is available. Horst will automatically retry to connect regularly |

Configuration File
==================

There is no configuration file.
All available configuration is done using command line parameters.

Commandline Parameters
=======================

All parameters have abbreviations with only their first character.

| Parameter             | Default value     | Value |
|-----------------------|-------------------|-------|
| --battery <threshold> | 70                | Battery threshold. If battery level fall below, we will go into safemode |
| --scripts <path>      | ./scripts/        | Path to scripts directory |
| --port <port>         | 99                | Listen port for S3TP |
| --socket <path>       | /run/s3tp.socket  | Socket path for S3TP |
| --manual              | false             | Start HORST in manual mode |
| --help                | -                 | Print short help |

D-Bus interface
===============

Horst has a D-Bus interface and listens on the following interface name.

| Name      | Value            |
|-----------|------------------|
| Name      | moveii.horst     |
| Path      | /moveii/horst    |
| Interface | moveii.horst     |

Methods
-------

| Method    | Description   |
|-----------|---------------|
| exec(s) | Run given shell command |
| setSafemode(s) | Toggle safemode on/off (False=off, True=on) |
| setManeuvermode(s) | Toggle maneuvermode on/off (False=off, True=on) |
| setManualmode(s) | Toggle manualmode on/off (False=off, True=on) |
| getBeaconData() | Get beacon data of daemon |
| checkDaemon() | Get status of beacon (always 0) |
| checkHardware() | Get hardware status of beacon (always 0) |
| getVersion() | Get version of daemon |

Signals
-------

Horst is listening for the following signals. All of them will update the
current state table accordingly.

| Signal    | Description   |
|-----------|---------------|
| thmStateChange | Update thm state |
| epsChargeStateChange | Update battery state |
| leopStateChange | Update leop state |
| payloadConditionsFullfilled | Update payload state |
| payloadMeasurementDone | Update payload state |
| adcsStateChange | Update adcs pointing state |

S3TP Interface
--------------

You can interact with HORST over S3TP.
The default socket path and S3TP port number can be changed per command
line parameters as documented above.

Over the S3TP interface, arbitrary shell commands can be executed on the
satellite. The returned value (string) will show success or failure of the
command execution, the stdout and stderr output of the executing command will be
transfered back to ground over S3TP.

Please keep in mind that you might want to discard most of the output by e.g.
redirecting it to /dev/null to save download capacity.

The easiest way to run remote commands is to use the remoteexec tool that
can be found in `/test/s3tp/`. Just give your command as parameter
and it will connect to HORST over s3tp, run your command and show
the results.

The protocol to communicate with HORST over S3TP is specified as follows:
1. S3TP connection is established as normal
2. Ground starts to send a command by sending its length as unsigned 64bit
   integer
3. Ground sends the command itself
4. As soon as HORST has received the expected number of bytes it will return
   an "ack" string to notify that it has successfully received the command.
5. HORST spawns a shell process with the given command
6. HORST returns stdout and stderr of the process over s3tp. Always first the
   number of bytes is send, afterwards the char array itself.
7. On exit of the process HORST returns the exit code as "[exit] 123" where 123
   is replaced by the actual return code

Every command may not be longer than 4096 bytes in total
(hardcoded buffer size for incoming data over S3TP).

E.g. a valid command would be:
```sh
./remoteexec ls -l
```
