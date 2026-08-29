// Case: dividing an ordinary quantity BY a decibel gain.
//
// expect: fail
// expect-match: cannot divide by a decibel gain
// expect-match: linear ratio
// expect-match: meters<
// forbid-match-gcc: candidate
#include <units/length.h>
#include <units/power.h>
using namespace units;
int main()
{
	units::meters<double> m(12.5); m /= units::decibels<double>(3.25); // ill-formed
	return 0;
}
