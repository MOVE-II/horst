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
| HORST-7 | Can't create listen address | Listen address could not be parsed | Check tcp port and try restarting the daemon |
| HORST-8 | Can't bind to socket | Daemon was not able to bind to tcp address and port | Recheck tcp listen port and restart daemon |
| HORST-9 | New connection error | Daemon was not able to accept new connection on tcp | Recheck tcp listen port and tcp connection and try again |
| HORST-10 | Failed to install the horst sdbus object | Daemon was not able to fully initialize his D-Bus functionality | Make sure D-Bus is working and restart daemon |
| HORST-11 | Failed to check LEOP status | The check_leop.sh script returned something differnt than 0 or 1 | Check script and restart daemon |

Configuration File
==================

There is no configuration file.

Commandline Parameters
=======================

All parameters have abbreviations with only their first character.

| Parameter            | Default value | Value |
|----------------------|---------------|-------|
| --battery <treshold> | 70            | Battery treshold. If battery level fall below, we will go into safemode |
| --scripts <path>     | ./scripts/    | Path to scripts directory |
| --port <port>        | 99            | Listen port for S3TP |
| --socket <path>      | /tmp/s3tp99   | Socket path for S3TP |
| --manual             | false         | Start HORST in manual mode |
| --help               | -             | Print short help |

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
The default socket path and S3TP port number can be changed over command
line parameters, as documented above.

Over the S3TP interface, arbitrary shell commands can be executed on the
satellite. The returned value (string) will show success or failure of the
command execution.

Every command must be proceeded by an "exec " and ends with a newline.
It may not be longer than 4096 bytes in total (hardcoded buffer size for
incoming data over S3TP).

E.g. a valid command would be:
```sh
exec ls\n
```

Returned will be an unsigned long integer (size_t) representing the number
of bytes following as char array (ASCII).
