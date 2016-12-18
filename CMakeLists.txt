cmake_minimum_required(VERSION 3.1)
project(horst CXX)

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
	src/action/procedure_call.cpp
	src/action/shell_command.cpp
	src/client/client.cpp
	src/client/tcp_client.cpp
	src/daemon.cpp
	src/error.cpp
	src/event/ctlmessage.cpp
	src/event/event.cpp
	src/event/req_daemon_control.cpp
	src/event/req_procedure_call.cpp
	src/event/req_shell_command.cpp
	src/horst.cpp
	src/log.cpp
	src/procedure/instruction/halt.cpp
	src/procedure/instruction/instruction.cpp
	src/procedure/instruction/loadc.cpp
	src/procedure/instruction/nop.cpp
	src/procedure/instruction/sleep.cpp
	src/procedure/procedure.cpp
	src/procedure/procedure_manager.cpp
	src/procedure/procedure_vm.cpp
	src/process.cpp
	src/satellite.cpp
	src/state/com.cpp
	src/state/computer.cpp
	src/state/eps.cpp
	src/state/state.cpp
	src/state/state_component.cpp
	src/state/thm.cpp
	src/util.cpp
)

target_link_libraries(horst
	uv
	systemd
	dl
)
