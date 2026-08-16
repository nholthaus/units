// Case: an unterminated separator literal ('%a'foo) in a LITERAL std::format spec is a compile error.
// expect: fail
// GCC/clang surface the thrown string; MSVC reports only C7595 for the consteval rejection. The message
// text is asserted portably at run time via EXPECT_THROW + what() in test/main.cpp.
// expect-match-gcc: units: unterminated separator literal
// expect-match-msvc: C7595
#include <format>
#include <units.h>
using namespace units::literals;
int main()
{
	(void)std::format("{:%a'foo}", 1.0_m);
	return 0;
}
