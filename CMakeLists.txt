cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

GET_FILENAME_COMPONENT(BRANCH ${CMAKE_CURRENT_SOURCE_DIR} NAME)
PROJECT(${BRANCH})
SET(TARGET_NAME ${BRANCH})

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set( ENV{CL} /MP )
endif()

SET(VERSION_GTEST "1.8.0" CACHE STRING "Google Test framework version")

enable_testing ()
add_subdirectory(3rdParty/gtest)
add_subdirectory(unitTests)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc ALL
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)