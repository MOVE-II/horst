*** Settings ***
Documentation    Horst integration tests
Resource    keywords/horst_keywords.robot
Resource    keywords/horst_keywords.robot

Test Setup    begin horst
Test Teardown    end horst

*** Test Cases ***
Trigger sun pointing
    [Tags]    Horst dbus interface
    Given that the system is in safemode 0
    And that the system manualmode is 0
    And the LEOP is in state UNDEPLOYED
    And the ADCS pointing is in state DETUMB
    When LEOP sends the state change signal DONE
    Then HORST should call ADCS setMode SUN
    And the ADCS requested pointing should be SUN

Trigger safemode when battery level below threshold
    Given that the system is in safemode 0
    And that the system manualmode is 0
    When EPS sends the charge state 25    # the value should be below the threshold
    Then the safemode is 1
    # systemctl stop pl.service Can't verify
    And HORST should call ADCS setMode SLEEP

Trigger safemode when temperature alarm is received
    Given that the system is in safemode 0
    And that the system manualmode is 0
    When THERMAL service sends state change signal ALARM
    Then the safemode is 1
    And HORST should call ADCS setMode SLEEP

