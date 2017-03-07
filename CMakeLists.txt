cmake_minimum_required(VERSION 3.1)
project(horst CXX)
set(PROJECT_BRIEF "Horst is the brain of our satellite.")

set(VERSION_MAJOR 2)
set(VERSION_MINOR 0)
include(version/version.cmake)

message("configuring horst...")

set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic") # -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(CMAKE_BUILD_TYPE DEBUG)


add_executable(horst
	src/action/action.cpp
	src/action/enter_manualmode.cpp
	src/action/enter_safemode.cpp
	src/action/leave_manualmode.cpp
	src/action/leave_safemode.cpp
	src/action/shell_command.cpp
	src/action/trigger_detumbling.cpp
	src/action/trigger_measuring.cpp
	src/action/trigger_sunpointing.cpp
	src/client/client.cpp
	src/client/tcp_client.cpp
	src/error.cpp
	src/event/ctlmessage.cpp
	src/event/eps_signal.cpp
	src/event/event.cpp
	src/event/manualmode_req.cpp
	src/event/manualmode_signal.cpp
	src/event/leop_signal.cpp
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
	src/server/tcp.cpp
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

target_link_libraries(horst
	uv
	systemd
	dl
)

install(
	TARGETS horst
	RUNTIME
	DESTINATION bin
)

# doxygen build
set(DOXYGEN_TARGETS horst)
include(doc/docs.cmake)
