#parent must define VERSION_GTEST

cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
PROJECT(gtest)
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

SET(VERSION_${PROJECT_NAME_UPPER} "?.?.?" CACHE STRING "${PROJECT_NAME} version string")
SET(${PROJECT_NAME_UPPER}_DIR ${CMAKE_CURRENT_BINARY_DIR}/src CACHE STRING "${PROJECT_NAME} source directory")

# ------------------------------------------------------------------------------------------------------------------------------
# LIBRARY LOCATION: Library location for various build types/generators
# ------------------------------------------------------------------------------------------------------------------------------

if((${CMAKE_GENERATOR} STREQUAL "Ninja") OR (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles"))
	SET(LIB_DIR ${${PROJECT_NAME_UPPER}_DIR}/googletest/${CMAKE_FIND_LIBRARY_PREFIXES}${PROJECT_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX})
else() # visual studio variants
	SET(DEBUG_LIB ${${PROJECT_NAME_UPPER}_DIR}/googletest/Debug/${CMAKE_FIND_LIBRARY_PREFIXES}${PROJECT_NAME}${CMAKE_FIND_LIBRARY_SUFFIXES})
	SET(RELEASE_LIB ${${PROJECT_NAME_UPPER}_DIR}/googletest/Release/${CMAKE_FIND_LIBRARY_PREFIXES}${PROJECT_NAME}${CMAKE_FIND_LIBRARY_SUFFIXES})
	SET(MINSIZEREL_LIB ${${PROJECT_NAME_UPPER}_DIR}/googletest/MinSizeRel/${CMAKE_FIND_LIBRARY_PREFIXES}${PROJECT_NAME}${CMAKE_FIND_LIBRARY_SUFFIXES})
	SET(RELWITHDEBINFO_LIB ${${PROJECT_NAME_UPPER}_DIR}/googletest/RelWithDebInfo/${CMAKE_FIND_LIBRARY_PREFIXES}${PROJECT_NAME}${CMAKE_FIND_LIBRARY_SUFFIXES})
	SET(LIB_DIR ${DEBUG_LIB} ${RELEASE_LIB} ${MINSIZEREL_LIB} ${RELWITHDEBINFO_LIB})
endif()

# ------------------------------------------------------------------------------------------------------------------------------
# EXTERNAL PROJECT: Definition of the external project to build the third-party library
# ------------------------------------------------------------------------------------------------------------------------------

IF(WIN32)
	SET(WINDOWS_CMAKE_ARGS -DCMAKE_CXX_FLAGS=/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
ENDIF(WIN32)

include(ExternalProject)
ExternalProject_Add(
	${PROJECT_NAME}-ext
	URL ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-${VERSION_${PROJECT_NAME_UPPER}}.zip
	URL_MD5 adfafc8512ab65fd3cf7955ef0100ff5
	DOWNLOAD_DIR ${${PROJECT_NAME_UPPER}_DIR}
	SOURCE_DIR ${${PROJECT_NAME_UPPER}_DIR}
	CMAKE_ARGS -DBUILD_GMOCK=OFF -DBUILD_GTEST=ON -Dgtest_force_shared_crt=ON ${WINDOWS_CMAKE_ARGS}
	INSTALL_COMMAND ""
	BUILD_IN_SOURCE 1
	BUILD_BYPRODUCTS ${LIB_DIR}
)

# ------------------------------------------------------------------------------------------------------------------------------
# EXPORT VARIABLES: Variables intended to help other projects find and use this library
# ------------------------------------------------------------------------------------------------------------------------------

# Library
add_library(${PROJECT_NAME} STATIC IMPORTED GLOBAL)
if((${CMAKE_GENERATOR} STREQUAL "Ninja") OR (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles"))
	SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES 
		IMPORTED_LOCATION ${LIB_DIR}
	)
else() # visual studio variants
	SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
		IMPORTED_LOCATION_DEBUG ${DEBUG_LIB}
		IMPORTED_LOCATION_RELEASE ${RELEASE_LIB}
		IMPORTED_LOCATION_MINSIZEREL ${MINSIZEREL_LIB}
		IMPORTED_LOCATION_RELWITHDEBINFO ${RELWITHDEBINFO_LIB}
	)
ENDIF()

# add pthreads on non-windows platforms
IF(WIN32)
ELSE(WIN32)
	SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES 
		INTERFACE_LINK_LIBRARIES -pthread
	)
ENDIF(WIN32)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-ext)

SET(INCLUDE_DIR ${${PROJECT_NAME_UPPER}_DIR}/googletest/include)
FILE(MAKE_DIRECTORY ${INCLUDE_DIR})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${INCLUDE_DIR})

MARK_AS_ADVANCED(${PROJECT_NAME_UPPER}_DIR)