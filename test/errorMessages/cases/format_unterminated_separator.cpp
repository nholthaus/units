// Case: an unterminated separator literal ('%a'foo) in a LITERAL std::format spec is a compile error.
// expect: fail
// expect-match: units: unterminated separator literal
#include <format>
#include <units.h>
using namespace units::literals;
int main()
{
	(void)std::format("{:%a'foo}", 1.0_m);
	return 0;
}
