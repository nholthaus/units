// Case: the compound spelling of scaling a reading by a dimensionless quantity.
//
// expect: fail
// expect-match: cannot scale an affine point
// expect-match: difference of two readings
// expect-match: celsius<
// forbid-match-gcc: candidate
#include <units/temperature.h>
#include <units/concentration.h>
using namespace units;
int main()
{
	units::temperature::celsius<double> c(20.5); c *= units::concentration::percent<double>(50.0); // ill-formed
	return 0;
}
