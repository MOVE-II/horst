*** Settings ***
Library    Process
Library    moveii.py

*** Keywords ***
get beacon data
    [Arguments]    ${service}
    ${raw_beacon_data}=    call service method    ${service}    getBeaconData
    @{beacon_data}=    beacon data as list  ${raw_beacon_data}
    [Return]    @{beacon_data}


call service method
    [Arguments]    ${service}    ${method}    ${data_type}=    ${value}=
    ${dbus_output}=    call service    moveii.${service}    /moveii/${service}    moveii.${service}    ${method}    ${data_type}    ${value}
    [Return]    ${dbus_output}


call service
    [Arguments]    ${service_name}    ${object_path}    ${service_name}    ${method}    ${data_type}    ${value}
    log    ${service_name} ${object_path} ${service_name} ${method} ${data_type} ${value}
    ${service_call_result}=    run process    busctl    --user    call    ${service_name}    ${object_path}    ${service_name}    ${method}    ${data_type}    ${value}
    [Return]    ${service_call_result.stdout}