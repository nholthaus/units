# Main concept of this test is a COMPILATION correctness test,
# rather than functional check.

PROJECT(unitLibTestFloat)

SET(SOURCE_CODE 
main.cpp
)
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_CODE})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARIES})

ADD_TEST(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})