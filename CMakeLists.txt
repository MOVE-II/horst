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
	src/client.cpp
	src/ctlmessage.cpp
	src/daemon.cpp
	src/error.cpp
	src/horst.cpp
	src/log.cpp
	src/satellite.cpp
	src/state.cpp
	src/task.cpp
	src/util.cpp
)

target_link_libraries(horst
	uv
	systemd
)
