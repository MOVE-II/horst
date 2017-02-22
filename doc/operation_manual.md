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

Configuration File
==================

There is no configuration file.
