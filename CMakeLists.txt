cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

PROJECT(units)

OPTION(BUILD_TESTS "Build unit tests" ON)
OPTION(BUILD_DOCS "Build the documentation" OFF)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# enable parallel build
	set( ENV{CL} /MP )
else()
	# turn off undefined inline function errors (occur in the traits classes)
	add_compile_options(-Wundefined-inline)
endif()

# header-only library target. To use this project as a subdirectory,
# add the following to your code:
#
# add_subdirectory(units) # or whatever you named the directory
# target_link_libraries(${PROJECT_NAME} units)
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# unit tests
if(BUILD_TESTS)
	set(VERSION_GTEST "1.8.0" CACHE STRING "Google Test framework version")
	enable_testing ()
	add_subdirectory(3rdParty/gtest)
	add_subdirectory(unitTests)
endif(BUILD_TESTS)

# add a target to generate API documentation with Doxygen
if(BUILD_DOCS)
	find_package(Doxygen)
	if(DOXYGEN_FOUND)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
		add_custom_target(doc ALL
			${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
			COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
	endif(DOXYGEN_FOUND)
endif(BUILD_DOCS)