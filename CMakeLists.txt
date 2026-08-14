cmake_minimum_required(VERSION 3.16...4.99 FATAL_ERROR)

PROJECT(units VERSION 3.4.0 LANGUAGES CXX)

# check if this is the main project
set(MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(MAIN_PROJECT ON)
endif()

# cmake options
OPTION(UNITS_BUILD_TESTS "Build unit tests" ${MAIN_PROJECT})
OPTION(UNITS_BUILD_DOCS "Build the documentation" OFF)
OPTION(UNITS_BUILD_EXAMPLES "Build the documentation examples" ${MAIN_PROJECT})
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

# Ship the MSVC debugger visualizer (natvis) so a consumer's Visual Studio shows a unit as "3 m" instead of an
# opaque object. Attaching it to the INTERFACE target propagates it to anything that links `units`, and MSVC
# auto-loads a .natvis carried on a target. No-op for non-MSVC generators.
if(MSVC)
	target_sources(${PROJECT_NAME} INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/natvis/units.natvis>
		$<INSTALL_INTERFACE:natvis/units.natvis>)
endif()

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
	add_subdirectory(test)
endif()

# documentation examples (compiled and run so the guides' snippets can never rot)
if(UNITS_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

# add a target to generate API documentation with Doxygen
if(UNITS_BUILD_DOCS)
	find_package(Doxygen)
	if(DOXYGEN_FOUND)
		# Treat Doxygen warnings as errors when asked (the docs CI sets DOXYGEN_WARN_AS_ERROR on pull
		# requests); default to NO so a local doc build is not gated. Accepts NO / YES / FAIL_ON_WARNINGS.
		if(DEFINED ENV{DOXYGEN_WARN_AS_ERROR})
			set(DOXYGEN_WARN_AS_ERROR "$ENV{DOXYGEN_WARN_AS_ERROR}")
		else()
			set(DOXYGEN_WARN_AS_ERROR "NO")
		endif()
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
		add_custom_target(doc ALL
				${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
				COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
	endif(DOXYGEN_FOUND)
endif()

# -----------------------------
# Install/export for find_package(units CONFIG)
# -----------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Version file for package config
write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/unitsConfigVersion.cmake"
		VERSION ${units_VERSION}
		COMPATIBILITY SameMajorVersion
)

# Install headers
install(DIRECTORY include/
		DESTINATION include
)

# Install the MSVC debugger visualizer so the installed package's $<INSTALL_INTERFACE:natvis/units.natvis>
# resolves and Visual Studio picks it up for consumers of the installed units package.
install(FILES natvis/units.natvis
		DESTINATION natvis
)

# Exported targets -> unitsTargets.cmake
install(TARGETS units
		EXPORT unitsTargets
		INCLUDES DESTINATION include
)

# Build-tree export to allow find_package with PATHS to the build dir (optional convenience)
export(EXPORT unitsTargets
		NAMESPACE units::
		FILE "${CMAKE_CURRENT_BINARY_DIR}/unitsTargets.cmake")

# Package config file generated from template (create cmake/unitsConfig.cmake.in alongside this CMakeLists)
configure_package_config_file(
		"${CMAKE_CURRENT_LIST_DIR}/cmake/unitsConfig.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/unitsConfig.cmake"
		INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/units"
		NO_SET_AND_CHECK_MACRO
		NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# Install exported targets and config files to a Debian/Ubuntu-friendly location
install(EXPORT unitsTargets
		NAMESPACE units::
		FILE unitsTargets.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/units
)

install(FILES
		"${CMAKE_CURRENT_BINARY_DIR}/unitsConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/unitsConfigVersion.cmake"
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/units
)

# -----------------------------
# CPack (packaging)
# -----------------------------
if(MAIN_PROJECT)
	# ---- CPack (Linux packages) ----

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
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE   "all")
	set(CPACK_DEBIAN_FILE_NAME              DEB-DEFAULT) # name_version-rev_arch
	set(CPACK_DEBIAN_PACKAGE_DEPENDS        " ")
	set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

	# RPM specifics (noarch for header-only)
	set(CPACK_RPM_PACKAGE_LICENSE          "MIT")
	set(CPACK_RPM_PACKAGE_NAME             "units-devel")
	set(CPACK_RPM_PACKAGE_GROUP            "Development/Libraries")
	set(CPACK_RPM_PACKAGE_ARCHITECTURE     "noarch")

	# Make sure CPack knows about this project
	include(CPack)
endif()
