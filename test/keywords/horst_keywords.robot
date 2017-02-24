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


the ADCS pointing is in detumb
    log    "pass"


the LEOP is in state ${leop_state}
    ${result}=    run process    python    dbus/leop.py    ${leop_state}
    ${leop}=    get LEOP from horst beacon
    log    ${leop}
    should be equal as integers    ${leop}    ${leop_state}

LEOP sends the state change signal done
    log    "pass"

HORST should call ADCS setMode SUN
    log    "pass"

the ADCS requested pointing should be SUN
    log    "pass"

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