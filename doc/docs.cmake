# Generate Doxygen documentation
find_package(Doxygen)

if(DOXYGEN_FOUND)
	find_file(DOXYGEN_FILE Doxyfile.in
		${CMAKE_CURRENT_SOURCE_DIR}/doc
	)

	set(DOXYGEN_INPUT "")
	set(DOXYGEN_INCLUDE_PATH "")
	foreach(_TARGET ${DOXYGEN_TARGETS})
		# Populate DOXYGEN_INPUT with all used source and public include files
		get_target_property(_LIST ${_TARGET} SOURCES)
		foreach(_FILE ${_LIST})
			set(DOXYGEN_INPUT "${DOXYGEN_INPUT} \\\n \"${_FILE}\"")
		endforeach(_FILE)
		get_target_property(_LIST ${_TARGET} INTERFACE_INCLUDE_DIRECTORIES)
		foreach(_FILE ${_LIST})
			set(DOXYGEN_INPUT "${DOXYGEN_INPUT} \\\n \"${_FILE}\"")
		endforeach(_FILE)

		# Populate DOXYGEN_INCLUDE_PATH for listing all libraries/includes
		get_target_property(_LIST ${_TARGET} INCLUDE_DIRECTORIES)
		foreach(_FILE ${_LIST})
			set(DOXYGEN_INCLUDE_PATH "${DOXYGEN_INCLUDE_PATH} \\\n \"${_FILE}\"")
		endforeach(_FILE)
	endforeach(_TARGET)

	configure_file(${DOXYGEN_FILE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating Doxygen documentation" VERBATIM
	)
else(DOXYGEN_FOUND)
	message(STATUS "Doxygen is required to build the documentation.")
endif(DOXYGEN_FOUND)
