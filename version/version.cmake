if(NOT DEFINED VERSION_MAJOR)
	message(FATAL_ERROR "VERSION_MAJOR is not set")
endif()
if(NOT DEFINED VERSION_MINOR)
	message(FATAL_ERROR "VERSION_MINOR is not set")
endif()

# assemble version variables
execute_process(
	COMMAND git log -1 --format=%h
	OUTPUT_VARIABLE VERSION_PATCH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
if ($ENV{CI})
    set(VERSION_STRING "${VERSION_STRING}-$ENV{CI_BUILD_ID}")
else()
    set(VERSION_STRING "${VERSION_STRING}-local")
endif()

# write into files
set(version_file_in "${CMAKE_CURRENT_SOURCE_DIR}/version/version.cpp.in")
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

configure_file(
	${version_file_in}
	${version_file}
)

message(STATUS "VERSION_STRING={${VERSION_STRING}}")

