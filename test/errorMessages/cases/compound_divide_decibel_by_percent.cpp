// Case: dividing a decibel level by a dimensionless quantity.
//
// expect: fail
// expect-match: cannot divide a decibel value
// expect-match: watts(level)
// expect-match: dBW<
// forbid-match-gcc: candidate
#include <units/power.h>
#include <units/concentration.h>
using namespace units;
int main()
{
	units::power::dBW<double> level(12.5); level /= units::concentration::percent<double>(50.0); // ill-formed
	return 0;
}
