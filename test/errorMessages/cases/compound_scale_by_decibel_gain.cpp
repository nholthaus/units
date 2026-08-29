// Case: scaling an ordinary quantity BY a decibel gain. A gain is a logarithmic figure, not a plain factor to multiply by.
//
// expect: fail
// expect-match: cannot scale by a decibel gain
// expect-match: linear ratio
// expect-match: meters<
// forbid-match-gcc: candidate
#include <units/length.h>
#include <units/power.h>
using namespace units;
int main()
{
	units::meters<double> m(12.5); m *= units::decibels<double>(3.25); // ill-formed
	return 0;
}
