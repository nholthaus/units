// Case: a duplicated label-form flag ('%aa') in a LITERAL std::format spec is a compile error.
// expect: fail
// expect-match: units: duplicate label-form flag
#include <format>
#include <units.h>
using namespace units::literals;
int main()
{
	(void)std::format("{:%aa}", 1.0_m);
	return 0;
}
