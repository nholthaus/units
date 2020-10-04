# ----------------------------------------------------------------------------------------------------------------------
# [USER-DEFINED] NOTES
# ----------------------------------------------------------------------------------------------------------------------

# Library:  googletest-1.10.0
# Author:   Nic Holthaus
#
# - Search this file for [USER-DEFINED] to easily find the sections that may need to be edited

# ----------------------------------------------------------------------------------------------------------------------
# [USER-DEFINED] PROJECT VARIABLES
# ----------------------------------------------------------------------------------------------------------------------

# ***** These are standardized project variables that need to be set based on the individual project *****
# ***** NOTE: These are not necessarily the only things you need to edit in this file if you're using it as a template for something else *****

# Package Extension: the extension of the compressed source package. 
# Hint: Either zip, tgz, or tar.gz
SET(PACKAGE_EXTENSION zip)

# The md5 sum of the compressed source package, used for integrity validation
# Hint: on linux or using git-bash you can find this value using the `md5sum` command
SET(PACKAGE_MD5 82358affdd7ab94854c8ee73a180fc53)

# Project Type: Set this to STATIC or SHARED depending on whether the build is for a static or shared library, respectively.
# Hint: static libraries usually have a .a or .lib extension, whereas shared libraries have a .so or .dll extension
SET(PROJECT_TYPE STATIC)

# Project libraries: The names of the libraries to be built if they are different from the project name. e.g. protobuf builds libprotobuf, libprotoc, and libprotobuf-lite
# Hint: comment this out if the project name is exactly the same as the library name.
LIST(APPEND PROJECT_LIBRARIES
    gtest
    gmock
)

# Project Link Libraries: Other libraries this one needs to link to or depends on
# Hint: Typically there are none. If there are no link dependencies, comment this list out.
LIST(APPEND PROJECT_LINK_LIBRARIES
  Threads::Threads
)

# Debug Postfix: set this to d library appends a 'd' to the library names when built in debug mode
# Hint: You may need to do a test debug build to figure this out.
SET(DEBUG_POSTFIX d)

# Project requires patch: set to true if the project needs to be patched before it's built. If TRUE, the project will
# search the current directory for a file called `patch.diff`, and use that to patch the project
SET(PROJECT_REQUIRES_PATCH FALSE)

# Project Source Subdir: set this if the CMakeLists.txt (or makefile, etc) isn't located in the base folder of the library package
# Hint: within the .zip just search for where the cmake or make file is. If it's not the root dir, you need to fill this out.
SET(PROJECT_SOURCE_SUBDIR )

# Project Source Subdir: set this if the include files aren't located in the base folder of the library package
# Hint: within the .zip just search for the top-level .h file of the library.
SET(PROJECT_INCLUDE_SUBDIR googletest/include)

# CMake arguments: additional arguments used by cmake IFF the build uses cmake as a generator
# Hint: you should find information about these in the project's original documentation or CMakeLists.txt file.
SET(PROJECT_CMAKE_ARGS -Dgtest_force_shared_crt=ON)

# Platform Specific CMake args: use this for additional cmake arguments that change depending on the OS or compiler, etc.
# Hint: you should find information about these in the project's original documentation or CMakeLists.txt file.
if(WIN32)
  SET(PLATFORM_SPECIFIC_CMAKE_ARGS )
elseif(APPLE)
  SET(PLATFORM_SPECIFIC_CMAKE_ARGS )
else()
  SET(PLATFORM_SPECIFIC_CMAKE_ARGS )
endif()

# ----------------------------------------------------------------------------------------------------------------------
# [BOILERPLATE] PROJECT VARIABLES
# ----------------------------------------------------------------------------------------------------------------------

# ***** This section is all boiler-plate, no editing required (or a good idea) *****

#----------------------------------------
# [BOILERPLATE] STANDARD CMAKE SETTINGS
#----------------------------------------

cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

#----------------------------------------
# [BOILERPLATE] STANDARD CMAKE VARIABLES
#----------------------------------------

# Use the global runtime and library locations if available, otherwise fill in some sane default values
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

#----------------------------------------
# [BOILERPLATE] PROJECT NAME
#----------------------------------------

# Determine the name and version of the library based on the repo/folder name
get_filename_component(DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE "-" ";" DIR_LIST ${DIR})

list(GET DIR_LIST 0 DIR_NAME)
list(GET DIR_LIST 1 DIR_VER)

# Set the project name
PROJECT(${DIR_NAME})

#----------------------------------------
# [BOILERPLATE] CACHE VARIABLES
#----------------------------------------

STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
SET(${PROJECT_NAME_UPPER}_DIR ${CMAKE_CURRENT_BINARY_DIR}/zip CACHE STRING "${PROJECT_NAME} source directory")
SET(VERSION_${PROJECT_NAME_UPPER} ${DIR_VER} CACHE STRING "${PROJECT_NAME} version string")

#----------------------------------------
# [BOILERPLATE] INTERNAL USE VARIABLES
#----------------------------------------

SET(VERSION ${VERSION_${PROJECT_NAME_UPPER}})

SET(PROJECT_DIR ${${PROJECT_NAME_UPPER}_DIR})
SET(PROJECT_URL ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-${VERSION}.${PACKAGE_EXTENSION})
SET(PROJECT_INCLUDE_DIR ${${PROJECT_NAME_UPPER}_DIR}/${PROJECT_INCLUDE_SUBDIR})
SET(PROJECT_LIBRARY_PREFIX ${CMAKE_${PROJECT_TYPE}_LIBRARY_PREFIX})
SET(PROJECT_LIBRARY_SUFFIX ${CMAKE_${PROJECT_TYPE}_LIBRARY_SUFFIX})
SET(PROJECT_EXECUTABLE_SUFFIX ${CMAKE_EXECUTABLE_SUFFIX})

SET(PROJECT_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR})
SET(PROJECT_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR})
SET(PROJECT_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR})

#----------------------------------------
# [BOILERPLATE] PATCH
#----------------------------------------

if(PROJECT_REQUIRES_PATCH)
  # Find a program that can apply a patch
  if(WIN32)
    find_package(Git REQUIRED)
    get_filename_component(GIT_DIR ${GIT_EXECUTABLE} DIRECTORY)
    find_program(PATCH patch.exe HINTS ${GIT_DIR}/../usr/bin)
  else()
    find_program(PATCH patch)
  endif()

  SET(PROJECT_PATCH_COMMAND ${PATCH} -p 1 -d ${PROJECT_DIR} -i ${CMAKE_CURRENT_SOURCE_DIR}/patch.diff --binary --forward --force --ignore-whitespace)
endif()

#----------------------------------------
# [BOILERPLATE] CREATE DIRECTORIES
#----------------------------------------

MAKE_DIRECTORY(${PROJECT_DIR})
MAKE_DIRECTORY(${PROJECT_INCLUDE_DIR})

#----------------------------------------
# [BOILERPLATE] LIBRARY NAMES
#----------------------------------------

# If the library names list is empty, append the project name
if(NOT PROJECT_LIBRARIES)
  LIST(APPEND PROJECT_LIBRARIES ${PROJECT_NAME})
endif()

#----------------------------------------
# [BOILERPLATE] STANDARD CMAKE ARGS
#----------------------------------------

# Forward the CMake build argument if there is one
if(NOT("${CMAKE_BUILD_TYPE}" STREQUAL ""))
	set(PROJECT_CMAKE_ARGS ${PROJECT_CMAKE_ARGS} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
endif()

# Forward the C++ Standard if one is defined
if(NOT("${CMAKE_CXX_STANDARD}" STREQUAL ""))
	set(PROJECT_CMAKE_ARGS ${PROJECT_CMAKE_ARGS} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD})
endif()

# Append the platform-specific cmake args
set(PROJECT_CMAKE_ARGS ${PROJECT_CMAKE_ARGS} ${PLATFORM_SPECIFIC_CMAKE_ARGS})

#----------------------------------------
# [BOILERPLATE] BYPRODUCTS
#----------------------------------------

# Mostly for Ninja compatibility
foreach(LIBRARY ${PROJECT_LIBRARIES})
  list(APPEND BYPRODUCTS
    ${PROJECT_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_LIBRARY_PREFIX}${LIBRARY}${PROJECT_LIBRARY_SUFFIX}
  )
  # can't use generator expressions here so grab the debug byproducts if necessary
  if(PROJECT_USES_D_FOR_DEBUG_LIBRARIES)
    list(APPEND BYPRODUCTS
      ${PROJECT_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_LIBRARY_PREFIX}${LIBRARY}d${PROJECT_LIBRARY_SUFFIX}
    )
  endif()
endforeach(LIBRARY ${PROJECT_LIBRARIES})

# ----------------------------------------------------------------------------------------------------------------------
# [USER-DEFINED] EXTERNAL PROJECT: Definition of the external project to build the third-party library.
# ----------------------------------------------------------------------------------------------------------------------

# ***** This section is intended to be mostly boilerplate, but may have to change for different 3rd Party libraries, especially if they don't use CMake. *****
# ***** Determining the build parameters is usually a trial-and-error endeavour. *****

# Refer to the CMake docs if necessary: https://cmake.org/cmake/help/latest/module/ExternalProject.html
# - For cmake projects, you may need to use the CMAKE_ARGS step
# - For MakeFile projects, you typically need a CONFIGURE_COMMAND and BUILD_COMMAND step
# - All projects need an install step to get the appropriate outputs into the  `PROJECT_XXX_OUTPUT_DIRECTORY` folders.
# - In some cases, it's necessary to have different `ExternalProject_Add` project calls for different OS's. If just,
#   just use `if(WIN32) ... elseif(APPLE) ... else() ... endif()`

# ---------------
# [BOILERPLATE] 
# ---------------
include(ExternalProject)
ExternalProject_Add(
  ${PROJECT_NAME}-ext                         
  URL ${PROJECT_URL}                          
  URL_MD5 ${PACKAGE_MD5}                      
  DOWNLOAD_DIR ${PROJECT_DIR}                 
  SOURCE_DIR ${PROJECT_DIR}                   
  SOURCE_SUBDIR  ${PROJECT_SOURCE_SUBDIR}     
  BUILD_IN_SOURCE TRUE
  BUILD_BYPRODUCTS ${BYPRODUCTS}
  PATCH_COMMAND ${PROJECT_PATCH_COMMAND}
  # ---------------
  # [USER-DEFINED] 
  # ---------------
  # Some additional commands may need to be added/removed here depending on what you're trying to build
  CMAKE_ARGS ${PROJECT_CMAKE_ARGS}            # This is boilerplate for a CMake project, should be commented out for other build systems
  INSTALL_COMMAND                             # Very user defined. The goal here is to get all the outputs of the build (.a, .lib, .so, .dll, .exe, etc) to the appropriate `PROJECT_XXX_OUTPUT_DIRECTORY`, depending on the file type
  COMMAND "${CMAKE_COMMAND}" -E make_directory ${PROJECT_ARCHIVE_OUTPUT_DIRECTORY} ${PROJECT_LIBRARY_OUTPUT_DIRECTORY} ${PROJECT_RUNTIME_OUTPUT_DIRECTORY}
  COMMAND "${CMAKE_COMMAND}" -E copy ${PROJECT_DIR}/lib/${CMAKE_CFG_INTDIR}/${PROJECT_LIBRARY_PREFIX}gtest$<$<CONFIG:Debug>:${DEBUG_POSTFIX}>${PROJECT_LIBRARY_SUFFIX} ${PROJECT_LIBRARY_OUTPUT_DIRECTORY}
  COMMAND "${CMAKE_COMMAND}" -E copy ${PROJECT_DIR}/lib/${CMAKE_CFG_INTDIR}/${PROJECT_LIBRARY_PREFIX}gmock$<$<CONFIG:Debug>:${DEBUG_POSTFIX}>${PROJECT_LIBRARY_SUFFIX} ${PROJECT_LIBRARY_OUTPUT_DIRECTORY}
)

# ----------------------------------------------------------------------------------------------------------------------
# [BOILERPLATE] LIBARY TARGETS: Creates the imported libraries for each target that was built
# ----------------------------------------------------------------------------------------------------------------------

set_target_properties (${PROJECT_NAME}-ext PROPERTIES FOLDER 3rdParty)

MESSAGE(DEBUG "${PROJECT_NAME} Include Directories: ${PROJECT_INCLUDE_DIR}")

# Library Target(s)
foreach(PROJECT_LIBRARY ${PROJECT_LIBRARIES})

  # Create the imported library
  add_library(${PROJECT_LIBRARY} ${PROJECT_TYPE} IMPORTED GLOBAL)
  SET_TARGET_PROPERTIES(${PROJECT_LIBRARY} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_INCLUDE_DIR}
  )
  if(NOT "${PROJECT_TYPE}" STREQUAL INTERFACE)
    SET(LOCATION ${PROJECT_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_LIBRARY_PREFIX}${PROJECT_LIBRARY}${PROJECT_LIBRARY_SUFFIX})
    SET(LOCATION_DEBUG ${PROJECT_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_LIBRARY_PREFIX}${PROJECT_LIBRARY}${DEBUG_POSTFIX}${PROJECT_LIBRARY_SUFFIX})

    SET_TARGET_PROPERTIES(${PROJECT_LIBRARY} PROPERTIES
      VERSION ${VERSION}
      IMPORTED_LOCATION ${LOCATION}
      IMPORTED_LOCATION_DEBUG ${LOCATION_DEBUG}
    )

    MESSAGE(DEBUG "${PROJECT_NAME} IMPORTED_LOCATION: ${PROJECT_INCLUDE_DIR}")
    MESSAGE(DEBUG "${PROJECT_NAME} IMPORTED_LOCATION_DEBUG: ${PROJECT_INCLUDE_DIR}")

  endif()

  # Add link libraries if there are any
  if(PROJECT_LINK_LIBRARIES)
    SET_TARGET_PROPERTIES(${PROJECT_LIBRARY} PROPERTIES
      INTERFACE_LINK_LIBRARIES ${PROJECT_LINK_LIBRARIES}
    )
  endif(PROJECT_LINK_LIBRARIES)

  # Add the IMPLIB if it's a shared library on windows
  if("${PROJECT_TYPE}" STREQUAL "SHARED" AND WIN32)
    SET_TARGET_PROPERTIES(${PROJECT_LIBRARY} PROPERTIES
      IMPORTED_IMPLIB ${PROJECT_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_LIBRARY}${CMAKE_STATIC_LIBRARY_SUFFIX}
      IMPORTED_IMPLIB_DEBUG ${PROJECT_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_LIBRARY}${DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
    )  
  endif()

  add_dependencies(${PROJECT_LIBRARY} ${PROJECT_NAME}-ext)

endforeach()

# Add a target to conglomerate all the seperate libraries (if there are more than one)
if(NOT TARGET ${PROJECT_NAME})
  add_library(${PROJECT_NAME} INTERFACE)
  set_target_properties(${PROJECT_NAME} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} INTERFACE ${PROJECT_LIBRARIES})
  add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-ext)
  LIST(APPEND PROJECT_LIBRARIES ${PROJECT_NAME})
endif()

