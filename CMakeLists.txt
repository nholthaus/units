cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

PROJECT(units VERSION 3.0.0 LANGUAGES CXX)

# check if this is the main project
set(MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(MAIN_PROJECT ON)
endif()

# cmake options
OPTION(UNITS_BUILD_TESTS "Build unit tests" ${MAIN_PROJECT})
OPTION(UNITS_BUILD_DOCS "Build the documentation" OFF)
OPTION(UNITS_DISABLE_IOSTREAM "Disables <iostream> (cout) support for embedded applications" OFF)
option(UNITS_CODE_COVERAGE "Generate coveralls code coverage data" OFF)

SET(CMAKE_CXX_STANDARD 17)

# header-only library target. To use this project as a subdirectory,
# add the following to your code:
#
# add_subdirectory(units) # or whatever you named the directory
# target_link_libraries(${PROJECT_NAME} units)
add_library(${PROJECT_NAME} INTERFACE)
add_library(units::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

# Remove IOStream from the library (useful for embdedded development)
if(UNITS_DISABLE_IOSTREAM)
	target_compile_definitions(${PROJECT_NAME} INTERFACE -DUNIT_LIB_DISABLE_IOSTREAM)
endif(UNITS_DISABLE_IOSTREAM)

# unit tests
include(CTest)
if(BUILD_TESTING AND UNITS_BUILD_TESTS)
	enable_testing()
	find_package(Threads)
	find_package(GTest 1.12.1)
	if (NOT GTest_FOUND)
		add_subdirectory(3rdParty/googletest-1.12.1)
	endif()
	add_subdirectory(unitTests)
endif()

# add a target to generate API documentation with Doxygen
if(UNITS_BUILD_DOCS)
	find_package(Doxygen)
	if(DOXYGEN_FOUND)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
		add_custom_target(doc ALL
			${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
			COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
	endif(DOXYGEN_FOUND)
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/unitsConfigVersion.cmake"
  VERSION ${units_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(TARGETS units
    EXPORT unitsConfig
)

install(EXPORT unitsConfig
    NAMESPACE units::
    DESTINATION share/units/cmake
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/unitsConfigVersion.cmake"
    DESTINATION share/units/cmake
)

install(DIRECTORY include/
    DESTINATION include
)