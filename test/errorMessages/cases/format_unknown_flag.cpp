// Case: an unknown unit-format flag in a LITERAL std::format spec is a compile error (the consteval
// basic_format_string check runs the formatter's parse(), which throws std::format_error).
// expect: fail
// expect-match: units: unknown unit-format flag
#include <format>
#include <units.h>
using namespace units::literals;
int main()
{
	(void)std::format("{:%z}", 1.0_m);
	return 0;
}
