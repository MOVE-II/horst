# System Design

Horst is the brain of the satellite, keeps track of a global state and reacts
on critical states by shutting off systems to safe battery.

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
| maneuvermode | true/false | false | If this is true the rules for setting ADCS mode to sun pointing will be deactivated |
| battery | Remaining voltage in percent | T | Remaining voltage of our battery |
| temperature | OK, WARN, ALARM | WARN | The temperature status of our whole satellite |
| ADCS pointing | \* | NONE | The currently achieved mode of the ADCS |
| ADCS pointing requested | \* | NONE | The currently requested mode of the ADCS |
| Payload | OFF, IDLE, WANTMEASURE, MEASURING | OFF | The current status of the payload daemon |
| LEOP | UNDEPLOYED, DEPLOYED, DONE | DEPLOYED | The status of the LEOP sequence, we are currently in |

\* Values for ADCS: NONE, SLEEP, ATTDET, DETUMB, NADIR, SUN, FLASH, EXP

All states can be changed by sending horst a signal with a new current value
or in some cases by a request (e.g. safemode and manual mode).
This will be done by the other daemons or requested by the ground station.

On startup HORST has no clue about the actual state of the satellite. It
therefore assumes the documented default values for operation until an update
with real data is received by signal.

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
| Finish leop         | finish\_leop.sh         | Power on the payload hardware |

For every action there is a corresponding script in the scripts directory
that will be run, whenever horst triggers the action.
Depending on the exit status of the script an internal state change might
be triggered (e.g. we set safemode to true, if the enter\_safemode.sh
script exists with 0).

The logic table
---------------

All rules that apply will trigger their corresponding action independant from other rules. Several rules may match on each iteration.
All scripts will first only be queued. Queued scripts will be processed after processing the whole logic table. 

| **Name** | **Request** | safemode | manualmode | maneuvermode | battery | temperature | ADCS pointing | ADCS requested pointing | PL | LEOP | **Action** |
|------|----------|-----|------------|--------------|---------|-------------|---------------|-------------------------|----|------|------------|
| Manualmode on (request) | manualmode on | - | no | - | - | - | - | - | - | - | enter\_manualmode.sh |
| Manualmode off (request) | manualmode off | - | yes | - | - | - | - | - | - | - | leave\_manualmode.sh |
| Battery low | - | no | no | - | <T | - | - | - | - | - | enter\_safemode.sh |
| Temperature ALARM | - | no | no | - | - | == alarm | - | - | - | - | enter\_safemode.sh |
| Safemode on (request) | safemode on | no | - | - | - | - | - | - | - | - | enter\_safemode.sh |
| Safemode off (request) | safemode off | yes | - | - | - | - | - | - | - | - | leave\_safemode.sh |
| Trigger PL measure | PL wantmeasure | no | no | X | >T | ok | - | - | != measuring | done | trigger\_measuring.sh |
| ADCS detumbling | - | no | no | no | - | - | !=sun and !=detumb | !=sun and !=detumb | - | != undeployed | trigger\_detumbling.sh |
| ADCS sunpointing | - | no | no | no | - | - | detumb | - | - | - | != undeployed | trigger\_sunpointing.sh |
| Leop done | - | - | - | - | - | - | - | - | - | != DONE | finish\_leop.sh |

Cells with a - will not be checked for the given rule.

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
