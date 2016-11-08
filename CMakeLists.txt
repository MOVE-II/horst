cmake_minimum_required(VERSION 3.1)
project(horst CXX)

set(CMAKE_CXX_STANDARD 14)


add_executable(horst
	src/daemon.cpp
	src/error.cpp
	src/horst.cpp
	src/log.cpp
	src/satellite.cpp
	src/state.cpp
	src/task.cpp
	src/util.cpp
)
