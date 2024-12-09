PROJECT(unitLibTest)

FILE(GLOB headers ../include/units/*.h)

SET(SOURCE_CODE
../include/units.h
${headers}
main.cpp
)

LIST(APPEND LIBRARIES
gtest
units
)

# coveralls support
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/3rdParty/coveralls)
if (UNITS_CODE_COVERAGE)

	# use absolute paths to help coveralls
	get_filename_component(MAIN main.cpp ABSOLUTE)
	get_filename_component(UNITS ../include/units.h ABSOLUTE)
    SET(FILES ${MAIN} ${UNITS})

	include(Coveralls)
	coveralls_turn_on_coverage()
	coveralls_setup("${FILES}" ON)
	
endif()

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_CODE})
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARIES})

IF(NOT MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wconversion -Wshadow)
ENDIF()

ADD_TEST(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

# use gtags if present on the system to populate the vscode intellisense
find_program(GTAGS name gtags HINTS /usr/local/bin DOC "Path to gtags parser")
if(GTAGS)
	add_custom_target(gtags COMMAND gtags WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMENT "Generating intellisense data...")
	add_dependencies(${PROJECT_NAME} gtags)
endif(GTAGS)


