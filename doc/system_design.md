# System Design

Horst is the brain of the satellite.

It keeps in internal state of the satellite and updates it on incoming signals
broadcasted by other daemons.
Based on the current state it checks a ruleset and runs some actions, if the
corresponding rule matches.


Available states
----------------

| State | Values | Default | Description |
|-------|--------|---------|-------------|
| manualmode | true/false | false | This toggles the logic of horst on/off to allow operators to disable horst for up to 30min. After this it will return to non-safemode automatically |
| safemode | true/false | false | If this is true the satellite is run in an energy saving mode |
| battery | Remaining voltage in 0.01% | T | Remaining voltage of our battery |
| temperature | ok, warn, alarm | warn | The temperature status of our whole satellite |
| ADCS pointing | \* | none | The currently achieved mode of the ADCS |
| ADCS pointing requested | \* | none | The currently requested mode of the ADCS |
| Payload | off, idle, wantmeasure, measuring | off | The current status of the payload daemon |
| LEOP | undeployed, deployed, done | done | The status of the LEOP sequence, we are currently in |

\* Values for ADCS: none, sleep, attdet, detumb, nadir, sun, flash, exp

All states can be changed by sending horst a signal with a new current value
or in some cases by a request (e.g. safemode and manual mode).
This will be done by the other daemons or requested by the ground station.

Resulting actions
-----------------

Horst might trigger the following actions, if rules apply.

| Action              | Script name             | Description |
|---------------------|-------------------------|-------------|
| Enter safemode      | enter\_safemode.sh      | Enter safemode by disabling certain components |
| Leave safemode      | leave\_safemode.sh      | Leave safemode by enabling certain components again |
| Enter manualmode    | enter\_manualmode.sh    |  Enter safemode and start timer to disable it again after 30 minutes |
| Leave manualmode    | leave\_manualmode.sh    | Leave manualmode (just resetting the mode) |
| Trigger measuring   | trigger\_measuring.sh   | Trigger measurement by the payload daemon |
| Trigger detumbling  | trigger\_detumbling.sh  | Trigger detumbling by the ADCS daemon |
| Trigger sunpointing | trigger\_sunpointing.sh | Trigger sunpointing by the ADCS daemon |

For every action there is a corresponding script in the scripts directory
that will be run, whenever horst triggers the action.
Depending on the exit status of the script an internal state change might
be triggered (e.g. we set safemode to true, if the enter\_safemode.sh
script exists with 0).

Algorithm
---------

HORST works the following way:

``` python

current_state = State()


while (True):
    # sleep until some change arrives
    event = event_loop_wait(s3tp, dbus, ...)

    # update the current state with a fact
    if event.is_fact():
        event.update(current_state)

    # or update the target state with a request
    target_state = current_state.copy()
    if not event.is_fact():
        event.update(target_state)

    # determine actions to perform when
    # we change from current to target state
    actions = current_state.transform_to(target_state)

    # enqueue those actions in the event loop
    event_loop_enqueue(actions)
```

The `current_state.transform_to` method does the actual work.
It determines what is necessary to come from `current_state`
to `target_state` by sending out requests. When requests are sent,
the `current_state` is updated to store the "request in progress".

The transformation is done with a table.
This table enforces constraints on the state.

If some new state is requested, the `target_state` is just set to the
desired outcome, but the transformation via this table is done afterwards.
This means for example if the power is not ok, no matter the request,
the transmitter will stay off.


| Temp ok | Power ok | transmit | mode | -> transmit | -> mode  |
|---------|----------|----------|------|-------------|----------|
| X       | 0        | _        | _    |           0 | fallback |
| 0       | X        | _        | _    |           0 | fallback |
| X       | X        | 1        | _    |           _ | speed    |
