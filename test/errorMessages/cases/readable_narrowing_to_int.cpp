// Case: a lossy conversion into an integer-underlying unit must FAIL readably, naming both types.
// feet -> meters is not an integer-exact ratio, so it cannot bind to meters<int> implicitly.
//
// expect: fail
// expect-match: feet<double>
// expect-match: meters<int>
#include <units/length.h>
using namespace units;
using namespace units::literals;
units::length::meters<int> a = 1.0_ft; // ill-formed: narrowing/lossy into an integer underlying
int main()
{
	(void)a;
	return 0;
}
