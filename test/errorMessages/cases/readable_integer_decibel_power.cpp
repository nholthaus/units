// Case: a power decibel unit (dBW) with an integral underlying type must FAIL readably, for the same
// reason as the dimensionless decibel — a logarithmic scale cannot use an integer underlying type.
//
// expect: fail
// expect-match: decibel-scale unit requires a floating-point underlying type
#include <units/power.h>
using namespace units;
units::power::dBW<int> a(10); // ill-formed: integral underlying type on a decibel scale
int main()
{
	(void)a;
	return 0;
}
