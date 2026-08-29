// Case: dividing a reading by a dimensionless quantity -- the same operation as dividing by a bare number.
//
// expect: fail
// expect-match: cannot divide an affine point
// expect-match: difference of two readings
// expect-match: celsius<
// forbid-match-gcc: candidate
#include <units/temperature.h>
#include <units/concentration.h>
using namespace units;
int main()
{
	auto bad = units::temperature::celsius<double>(20.5) / units::concentration::percent<double>(50.0); (void)bad; // ill-formed
	return 0;
}
