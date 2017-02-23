*** Settings ***
Library    Process
Library    OperatingSystem


*** Variables ***
${DBUS_CALL}    busctl --user call moveii.horst


*** Keywords ***
that the system is not in safemode
    call dbus moveii/horst moveii.horst setSafemode

that it is not in manualmode
    log    "pass"


the ADCS pointing is in detumb
    log    "pass"


the LEOP is in undeployed state
    log    "pass"


LEOP sends the state change signal done
    log    "pass"


HORST should call ADCS setMode SUN
    log    "pass"

the ADCS requested pointing should be SUN
    log    "pass"


call dbus ${object_path} ${service_name} ${method} ${params}
    ${result}=    run process    ${DBUS_CALL}    ${object_path}    ${service_name}    ${method}    ${params}

begin ${application}
    set environment variable    LD_LIBRARY_PATH    %{LD_LIBRARY_PATH}:/usr/local/lib
    ${handle}=    Start Process    ${application}
    set test variable    ${handle}

end ${application}
    sleep    10s
    ${result}=    terminate process    ${handle}
    log    ${result.stdout}