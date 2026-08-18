cmake_minimum_required(VERSION 3.16...4.99 FATAL_ERROR)

PROJECT(units VERSION 3.6.1 LANGUAGES CXX)

include(GNUInstallDirs)

# check if this is the main project
set(MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(MAIN_PROJECT ON)
endif()

# cmake options
OPTION(UNITS_BUILD_TESTS "Build unit tests" ${MAIN_PROJECT})
OPTION(UNITS_BUILD_DOCS "Build the documentation" OFF)
# The examples are compiled artifacts for development and CI, like the tests — a consumer of this header-only
# library never needs them built. They therefore follow UNITS_BUILD_TESTS: on for a repo/developer build, and off
# whenever a packaging build turns tests off (a Conan/vcpkg recipe passes -DUNITS_BUILD_TESTS=OFF), so a package
# install compiles nothing and stages only the headers.
OPTION(UNITS_BUILD_EXAMPLES "Build the documentation examples" ${UNITS_BUILD_TESTS})
# Generate install/export rules. On by default when units is the top-level project (a package build: CPack, a
# Conan/vcpkg recipe that configures units directly), off when units is pulled in via add_subdirectory so a
# consuming application's `cmake --install` does not also stage units' headers and CMake package files. A packager
# that DOES vendor units as a subdirectory and still wants it installed can force it on with -DUNITS_INSTALL=ON.
OPTION(UNITS_INSTALL "Generate the install/export rules for units" ${MAIN_PROJECT})
OPTION(UNITS_DISABLE_IOSTREAM "Disables <iostream> (cout) support for embedded applications" OFF)
OPTION(UNITS_DISABLE_FORMAT "Disables std::format / std::print support (keeps iostream and to_string)" OFF)
OPTION(UNITS_DISABLE_STRING "Disables all <string> use (leanest build; implies DISABLE_IOSTREAM and DISABLE_FORMAT)" OFF)

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
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_DATADIR}/units/natvis/units.natvis>)
endif()

# Text-feature opt-outs (all default OFF = full capability). See the "TEXT-FEATURE CONFIGURATION" block in
# include/units/core.h for the exact semantics and backward-compatibility guarantees.
if(UNITS_DISABLE_IOSTREAM)
	target_compile_definitions(${PROJECT_NAME} INTERFACE UNIT_LIB_DISABLE_IOSTREAM)
endif(UNITS_DISABLE_IOSTREAM)

if(UNITS_DISABLE_FORMAT)
	target_compile_definitions(${PROJECT_NAME} INTERFACE UNIT_LIB_DISABLE_FORMAT)
endif(UNITS_DISABLE_FORMAT)

if(UNITS_DISABLE_STRING)
	target_compile_definitions(${PROJECT_NAME} INTERFACE UNIT_LIB_DISABLE_STRING)
endif(UNITS_DISABLE_STRING)

# unit tests. include(CTest) — which defines the BUILD_TESTING option (default ON) and adds the Dart/CTest
# targets — is pulled in only when units is actually building its own tests, so a consumer that adds units via
# add_subdirectory does not inherit units' testing scaffolding in their cache or target list.
if(UNITS_BUILD_TESTS)
	include(CTest)
endif()
if(UNITS_BUILD_TESTS AND BUILD_TESTING)
	find_package(Threads)
	set(GTEST_VERSION 1.17.0)
	find_package(GTest ${GTEST_VERSION} QUIET CONFIG)
	if (NOT GTest_FOUND)
		message(STATUS "No system googletest found. Using project-provided gtest ${GTEST_VERSION}")
		set(INSTALL_GTEST OFF)
		add_subdirectory(3rdParty/googletest-1.17.0)
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
# Guarded by UNITS_INSTALL (default = top-level project) so a consuming application that vendors units via
# add_subdirectory does not have units' headers and package files injected into its own `cmake --install`.
if(UNITS_INSTALL)
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

# Install the debugger visualizers: the MSVC natvis (so the installed target's shared-data path resolves and
# Visual Studio picks it up) and the LLDB Python
# formatter (loaded manually via `command script import`; shipped so consumers of the installed package have it).
install(FILES natvis/units.natvis natvis/units_lldb.py
		DESTINATION ${CMAKE_INSTALL_DATADIR}/units/natvis
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
endif() # UNITS_INSTALL

# -----------------------------
# CPack (packaging)
# -----------------------------
# Requires the install rules: without them CPack would produce an empty package (headers and CMake files come from
# the install(...) commands above), so packaging is gated on UNITS_INSTALL as well as being the top-level project.
if(MAIN_PROJECT AND UNITS_INSTALL)
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