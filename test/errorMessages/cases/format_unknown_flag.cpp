// Case: an unknown unit-format flag in a LITERAL std::format spec is a compile error (the consteval
// basic_format_string check runs the formatter's parse(), which throws std::format_error).
// expect: fail
// GCC/clang surface the thrown std::format_error string; MSVC reports only C7595 (call to immediate
// function is not a constant expression) for the same consteval rejection. The exact message text is also
// asserted portably at run time via EXPECT_THROW + what() in test/main.cpp.
// expect-match-gcc: units: unknown unit-format flag
// expect-match-msvc: C7595
#include <format>
#include <units.h>
using namespace units::literals;
int main()
{
	(void)std::format("{:%z}", 1.0_m);
	return 0;
}
