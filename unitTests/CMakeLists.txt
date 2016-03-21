SET(TARGET_NAME unitLibTest)
PROJECT(${TARGET_NAME})	# it's a pun, GET IT!?!?

SET(SOURCE_CODE 
main.cpp
${CMAKE_CURRENT_SOURCE_DIR}/../include/units.h
)

LIST(APPEND INCLUDE_DIRS
../include
${GTEST_INCLUDE_DIRS}
)

LIST(APPEND LIBRARIES
${GTEST_LIBRARIES}
)

if(WIN32)
else(WIN32)
	LIST(APPEND CMAKE_CXX_FLAGS -std=c++14)
endif(WIN32)

# coveralls support
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/3rdParty/coveralls)
option(COVERALLS "Generate coveralls data" OFF)
if (COVERALLS)
	include(Coveralls)
	coveralls_turn_on_coverage()
	coveralls_setup("${SOURCE_CODE}" # The source files.
		ON   
	)
endif()

include_directories(${INCLUDE_DIRS})
ADD_EXECUTABLE(${TARGET_NAME} ${SOURCE_CODE})
ADD_TEST(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
TARGET_LINK_LIBRARIES(${TARGET_NAME} ${LIBRARIES})

IF(WIN32)
	set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/machine:x64")
ENDIF(WIN32)
