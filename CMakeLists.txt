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
	src/action/shell_command.cpp
	src/client.cpp
	src/daemon.cpp
	src/error.cpp
	src/event/ctlmessage.cpp
	src/event/event.cpp
	src/horst.cpp
	src/log.cpp
	src/procedure/procedure.cpp
	src/procedure/procedure_step.cpp
	src/satellite.cpp
	src/state/computer.cpp
	src/state/state.cpp
	src/state/state_component.cpp
	src/util.cpp
)

target_link_libraries(horst
	uv
	systemd
	dl
)
