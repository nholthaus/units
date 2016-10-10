PROJECT(unitLibTest)

# use absolute paths to help coveralls
get_filename_component(MAIN main.cpp ABSOLUTE)
get_filename_component(INC ../include/units.h ABSOLUTE)

SET(SOURCE_CODE 
${MAIN}
${INC}
)

LIST(APPEND INCLUDE_DIRS
../include
)

LIST(APPEND LIBRARIES
gtest
)

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
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_CODE})
ADD_TEST(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARIES})

# use gtags if present on the system to populate the vscode intellisense
find_program(GTAGS name gtags HINTS /usr/local/bin DOC "Path to gtags parser")
if(GTAGS)
	add_custom_target(gtags COMMAND gtags WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMENT "Generating intellisense data...")
	add_dependencies(${PROJECT_NAME} gtags)
endif(GTAGS)

IF(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/machine:x64")
ENDIF(WIN32)
