cmake_minimum_required(VERSION 3.16...4.99 FATAL_ERROR)

PROJECT(units VERSION 3.1.1 LANGUAGES CXX)

# check if this is the main project
set(MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(MAIN_PROJECT ON)
endif()

# cmake options
OPTION(UNITS_BUILD_TESTS "Build unit tests" ${MAIN_PROJECT})
OPTION(UNITS_BUILD_DOCS "Build the documentation" OFF)
OPTION(UNITS_DISABLE_IOSTREAM "Disables <iostream> (cout) support for embedded applications" OFF)

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

# Require C++23 for consumers (header-only propagation)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

# Remove IOStream from the library (useful for embedded development)
if(UNITS_DISABLE_IOSTREAM)
	target_compile_definitions(${PROJECT_NAME} INTERFACE UNIT_LIB_DISABLE_IOSTREAM)
endif(UNITS_DISABLE_IOSTREAM)

# unit tests
include(CTest)
if(BUILD_TESTING AND UNITS_BUILD_TESTS)
	find_package(Threads)
	set(GTEST_VERSION 1.17.0)
	find_package(GTest ${GTEST_VERSION} QUIET CONFIG)
	if (NOT GTest_FOUND)
		message(STATUS "No system googletest found. Using project-provided gtest ${GTEST_VERSION}")
		add_subdirectory(3rdParty/googletest-1.170.0)
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

# To use this project with find_package, do
# find_package(units CONFIG REQUIRED)
# target_link_libraries(myapp PRIVATE units::units)
install(TARGETS units
		EXPORT unitsConfig
		INCLUDES DESTINATION include
)

# downstream projects can do find_package(units CONFIG PATHS
#   "${CMAKE_CURRENT_BINARY_DIR}/share/units/cmake"
#   NO_DEFAULT_PATH) before installation occurs
export(EXPORT unitsConfig
		NAMESPACE units::
		FILE "${CMAKE_CURRENT_BINARY_DIR}/unitsConfig.cmake")

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

# ---- CPack (Linux packages) ----
include(GNUInstallDirs)

# Choose generators based on host capabilities
set(CPACK_GENERATOR "")  # start empty

# Always offer a tarball as a fallback
list(APPEND CPACK_GENERATOR "TGZ")

# DEB if this looks like Debian/Ubuntu
if (EXISTS "/etc/debian_version")
	list(APPEND CPACK_GENERATOR "DEB")
endif()

# RPM only if rpmbuild is present
find_program(RPMBUILD_EXECUTABLE rpmbuild)
if (RPMBUILD_EXECUTABLE)
	list(APPEND CPACK_GENERATOR "RPM")
endif()

set(CPACK_PACKAGE_NAME             "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR           "Nicolas Holthaus")
set(CPACK_PACKAGE_CONTACT          "10323743+nholthaus@users.noreply.github.com")
set(CPACK_PACKAGE_HOMEPAGE_URL     "https://github.com/nholthaus/units")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Header-only C++ units library")
set(CPACK_PACKAGE_VERSION          "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")

# Put files under the usual prefix when installing from a package
set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# License file if you have one at repo root
if(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
endif()

# Debian specifics (header-only → architecture independent)
# Debian/Ubuntu naming & conventions for header-only libs
set(CPACK_DEBIAN_PACKAGE_NAME           "libunits-dev")
set(CPACK_DEBIAN_PACKAGE_SECTION        "libdevel")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE   "all")
set(CPACK_DEBIAN_FILE_NAME              DEB-DEFAULT) # name_version-rev_arch
set(CPACK_DEBIAN_PACKAGE_DEPENDS        " ")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

# RPM naming for header-only libs
set(CPACK_RPM_PACKAGE_NAME              "units-devel")
set(CPACK_RPM_PACKAGE_GROUP             "Development/Libraries")
set(CPACK_RPM_PACKAGE_ARCHITECTURE      "noarch")

# RPM specifics (noarch for header-only)
set(CPACK_RPM_PACKAGE_LICENSE          "MIT")
set(CPACK_RPM_PACKAGE_GROUP            "Development/Libraries")
set(CPACK_RPM_PACKAGE_ARCHITECTURE     "noarch")
set(CPACK_RPM_PACKAGE_REQUIRES         " ")

# Make sure CPack knows about this project
include(CPack)
