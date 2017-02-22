Error Codes
===========

|   Code   |      Text      |  Description  | Next steps |
|----------|----------------|---------------|------------|
| HORST-1 | No error codes yet | | |

Configuration File
==================

There is no configuration file yet.

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
| run(s) | Run procedure with given name |
| set(s) | Some mystic debugging stuff |
| safemode(y) | Toggle safemode on/off (0=off, 1=on) |
| manualmode(y) | Toggle manualmode on/off (0=off, 1=on) |
| getVersion() | Get version of daemon |

Signals
-------

Horst is listening for the following signals. All of them will update the
current state table accordingly.

| Signal    | Description   |
|-----------|---------------|
| thmStateChange | Update thm state |
| epsChargeStateChange | Update battery state |
| LeopStateChange | Update leop state |
| PayloadMeasurementDone | Update payload state |
| adcsStateChange | Update adcs pointing state |
