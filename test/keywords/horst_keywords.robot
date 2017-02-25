*** Settings ***
Library    Process
Library    OperatingSystem
Library    horst_utils.py
Resource    dbus.robot


*** Variables ***


*** Keywords ***
that the system is in safemode ${mode}
    set safemode    ${mode}
    ${safemode}=    get safemode from horst beacon
    log    Safemode is ${safemode}
    should be equal as integers    ${safemode}    ${mode}

that the system manualmode is ${mode}
    set manualmode    ${mode}
    ${manualmode}=    get manualmode from horst beacon
    log    Manualmode is ${manualmode}
    should be equal as integers    ${manualmode}    ${mode}


the ADCS pointing is in state ${pointing_state}
    ${result}=    run process    python    dbus/leop.py    ${pointing_state}
    ${pointing}=    get adcs_pointing from horst beacon
    ${pointing_as_string}=    adcs state as string    ${pointing}
    log    ${pointing_as_string}
    should be equal as strings    ${pointing_as_string}    ${pointing_state}


the LEOP is in state ${leop_state}
    send leop state change    ${leop_state}

LEOP sends the state change signal ${state}
    ${handle}=    Start Process    python    dbus/adcs_service.py
    sleep    1s
    send leop state change    ${state}
    sleep  1s
    Terminate Process    ${handle}


HORST should call ADCS setMode ${expected_mode}
    ${actual_mode}=    get file    dbus/adcs_result.txt
    log    ${actual_mode}
    remove file    dbus/adcs_result.txt
    should be equal as strings    ${actual_mode}    ${expected_mode}


the ADCS requested pointing should be ${expected_state}
    ${state_id}=    get adcs_requested from horst beacon
    ${actual_state}=    adcs state as string    ${state_id}
    log    ${actual_state}
    should be equal as strings    ${actual_state}    ${expected_state}


EPS sends the charge state ${charge_level}
    ${handle}=    Start Process    python    dbus/adcs_service.py
    sleep    1s
    ${result}=    run process    python    dbus/eps.py    ${charge_level}
    sleep    2s
    Terminate Process    ${handle}
    ${battery_MSB}=    get battery_msb from horst beacon
    ${battery_LSB}=    get battery_lsb from horst beacon
    ${battery_lvl}=    integer from bytes    ${battery_MSB}    ${battery_LSB}
    log    ${battery_lvl}
    should be equal as integers    ${battery_lvl}    ${charge_level}

the safemode is ${expected_safemode}
    ${actual_safemode}=    get safemode from horst beacon
    log    Safemode is ${actual_safemode}
    should be equal as integers    ${actual_safemode}    ${expected_safemode}

THERMAL service sends state change signal ${new_state}
    ${handle}=    Start Process    python    dbus/adcs_service.py
    sleep    1s
    ${result}=    run process    python    dbus/thermal.py    ${new_state}
    sleep    2s
    Terminate Process    ${handle}
    ${thermal_state_id}=    get thermal from horst beacon
    ${thermal_state_string}=    get thermal state as string    ${thermal_state_id}
    log    ${thermal_state_string}
    should be equal as strings    ${thermal_state_string}    ${new_state}


send leop state change
    [Arguments]    ${new_state}
    ${new_state_id}=    leop state id    ${new_state}
    ${result}=    run process    python    dbus/leop.py    ${new_state_id}
    ${actual_state}=    get LEOP from horst beacon
    log    ${actual_state}
    should be equal as integers    ${actual_state}    ${new_state_id}

get ${field} from horst beacon
    ${raw_beacon_data}=    get beacon data    horst
    &{beacon_data}=    horst beacon data    ${raw_beacon_data}
    [Return]    &{beacon_data}[${field}]

set safemode
    [Arguments]    ${mode}
    call service method    horst    safemode    y    ${mode}

set manualmode
    [Arguments]    ${mode}
    call service method    horst    manualmode    y    ${mode}

begin ${application}
    set environment variable    LD_LIBRARY_PATH    %{LD_LIBRARY_PATH}:/usr/local/lib
    ${horst_handle}=    Start Process    ${application}
    sleep    5s
    set test variable    ${horst_handle}

end ${application}
    ${result}=    terminate process    ${horst_handle}
    log    ${result.stdout}