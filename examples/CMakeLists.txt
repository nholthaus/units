PROJECT(unitExamples)

# Every example is a tiny, self-contained program that must compile and run under C++23. They double as
# the documentation's source of truth: the guides quote these files via Doxygen \snippet, so a snippet
# can never drift from code that actually builds. Building them is gated on UNITS_BUILD_EXAMPLES.

SET(EXAMPLES
		hello_units
		dimensional_analysis
		pythagoras
		dimensionless_percent
		chrono_interop
		defining_a_unit
		json_roundtrip
)

FOREACH (example ${EXAMPLES})
	ADD_EXECUTABLE(${example} ${example}.cpp)
	TARGET_LINK_LIBRARIES(${example} units)
	IF (NOT MSVC)
		TARGET_COMPILE_OPTIONS(${example} PRIVATE -Wall -Wextra)
	ELSEIF (MSVC)
		TARGET_COMPILE_OPTIONS(${example} PRIVATE /permissive-)
	ENDIF ()
	# Run each example under ctest so CI proves it not only compiles but executes cleanly (exit 0).
	ADD_TEST(NAME example_${example} COMMAND ${example})
ENDFOREACH ()
