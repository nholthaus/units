// Case: the hyperbolic functions take a dimensionless argument; calling cosh() on a length must FAIL
// readably, naming the offending strong type and the dimensionless constraint it did not satisfy.
//
// expect: fail
// expect-match: meters<double>
// expect-match: dimensionless
#include <units/angle.h>
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto bad = cosh(1.0_m); // ill-formed: cosh expects a dimensionless value, not a length
int main()
{
	(void)bad;
	return 0;
}
