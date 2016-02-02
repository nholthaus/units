cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

GET_FILENAME_COMPONENT(BRANCH ${CMAKE_CURRENT_SOURCE_DIR} NAME)
PROJECT(${BRANCH})
SET(TARGET_NAME ${BRANCH})

set( ENV{CL} /MP )

SET(VERSION_GTEST "1.7.0" CACHE STRING "Google Test framework version")

enable_testing ()

add_subdirectory(3rdParty/gnuWin)
add_subdirectory(3rdParty/gtest)

add_subdirectory(unitTests)

