// Case: assigning an area (m^2) to a length (m) must FAIL readably, naming both friendly types.
//
// expect: fail
// expect-match: square_meters
// expect-match: meters
#include <units/length.h>
#include <units/area.h>
using namespace units;
using namespace units::literals;
units::length::meters<double> a = 1.0_m * 1.0_m; // ill-formed: m*m is an area, not a length
int main()
{
	(void)a;
	return 0;
}
