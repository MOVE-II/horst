# export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
*** Settings ***
Documentation    Horst integration tests
Resource    keywords/horst_keywords.robot
Resource    keywords/horst_keywords.robot

Test Setup    begin horst
Test Teardown    end horst

*** Test Cases ***
Trigger sun pointing
    [Tags]    Horst dbus interface
    Given that the system is not in safemode
    And that it is not in manualmode
    And the ADCS pointing is in detumb
    And the LEOP is in undeployed state
    When LEOP sends the state change signal done
    Then HORST should call ADCS setMode SUN
    And the ADCS requested pointing should be SUN