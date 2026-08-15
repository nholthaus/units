// Case: a decibel-scale unit with an integral underlying type must FAIL readably.
// A decibel stores its value through a base-10 logarithm, so an integer cannot represent it (3 dB would
// store as 0, large values overflow). The static_assert names the reason.
//
// expect: fail
// expect-match: decibel-scale unit requires a floating-point underlying type
#include <units/core.h>
using namespace units;
units::decibels<int> a(3); // ill-formed: integral underlying type on a decibel scale
int main()
{
	(void)a;
	return 0;
}
