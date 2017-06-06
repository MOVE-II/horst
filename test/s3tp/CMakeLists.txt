cmake_minimum_required(VERSION 3.4)
project(HORSTs3tptest)
set(PROJECT_BRIEF "HORST S3TP test program")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 3)

# packages
find_package(trctrl REQUIRED)
include_directories(${TRCTRL_INCLUDE_DIRS})

find_package(s3tp REQUIRED)
include_directories(${S3TP_INCLUDE_DIRS})

find_package(s3tp-core REQUIRED)
include_directories(${S3TP_INCLUDE_DIRS})

find_library(systemd systemd)
find_library(systemd++ systemd++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

#connector
set(CONNECTOR_SRC_FILES remoteexec.cpp)

add_executable(remoteexec ${CONNECTOR_SRC_FILES})

target_link_libraries(remoteexec ${S3TP_LIBRARY})
target_link_libraries(remoteexec pthread)

install(TARGETS remoteexec RUNTIME DESTINATION bin)
