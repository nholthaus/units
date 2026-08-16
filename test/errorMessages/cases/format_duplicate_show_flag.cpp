// Case: a duplicated show flag ('%vu') in a LITERAL std::format spec is a compile error.
// expect: fail
// expect-match: units: duplicate show flag
#include <format>
#include <units.h>
using namespace units::literals;
int main()
{
	(void)std::format("{:%vu}", 1.0_m);
	return 0;
}
