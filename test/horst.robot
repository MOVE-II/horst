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
    And the LEOP is in state 0
    And the ADCS pointing is in detumb
    When LEOP sends the state change signal done
    Then HORST should call ADCS setMode SUN
    And the ADCS requested pointing should be SUN