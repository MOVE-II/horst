cmake_minimum_required(VERSION 3.1)
project(horst CXX)
set(PROJECT_BRIEF "Horst is the brain of Move2 satellite.")

set(VERSION_MAJOR 2)
set(VERSION_MINOR 1)
include(version/version.cmake)

message("configuring horst...")

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(s3tp REQUIRED)
include_directories(${S3TP_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic") # -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(CMAKE_BUILD_TYPE RELEASE)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/action")
include_directories("${PROJECT_SOURCE_DIR}/client")
include_directories("${PROJECT_SOURCE_DIR}/event")
include_directories("${PROJECT_SOURCE_DIR}/server")

add_executable(horst
	src/action/action.cpp
	src/action/enter_manualmode.cpp
	src/action/enter_safemode.cpp
	src/action/finish_leop.cpp
	src/action/leave_manualmode.cpp
	src/action/leave_safemode.cpp
	src/action/shell_command.cpp
	src/action/trigger_detumbling.cpp
	src/action/trigger_measuring.cpp
	src/action/trigger_sunpointing.cpp
	src/client/client.cpp
	src/error.cpp
	src/event/ctlmessage.cpp
	src/event/eps_signal.cpp
	src/event/event.cpp
	src/event/manualmode_req.cpp
	src/event/manualmode_signal.cpp
	src/event/leop_signal.cpp
	src/event/leop_req.cpp
	src/event/adcs_signal.cpp
	src/event/adcs_req_signal.cpp
	src/event/payload_req.cpp
	src/event/payload_signal.cpp
	src/event/req_daemon_control.cpp
	src/event/req_shell_command.cpp
	src/event/safemode_req.cpp
	src/event/safemode_signal.cpp
	src/event/thm_signal.cpp
	src/horst.cpp
	src/process.cpp
	src/satellite.cpp
	src/server/dbus.cpp
	src/server/s3tp.cpp
	src/state/adcs.cpp
	src/state/com.cpp
	src/state/computer.cpp
	src/state/eps.cpp
	src/state/payload.cpp
	src/state/state.cpp
	src/state/state_component.cpp
	src/state/thm.cpp
	src/util.cpp
	${version_file}
)

target_link_libraries(horst uv)
target_link_libraries(horst systemd)
target_link_libraries(horst dl)
target_link_libraries(horst ${S3TP_LIBRARY})

install(
	TARGETS horst
	RUNTIME
	DESTINATION bin
)

# doxygen build
set(DOXYGEN_TARGETS horst)
include(doc/docs.cmake)
