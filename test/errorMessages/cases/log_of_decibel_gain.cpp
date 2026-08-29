// Case: a transcendental function reads a quantity's VALUE, which on a logarithmic scale is the decibel figure rather
// than the ratio it denotes. `log10(decibels(3.25))` returned log10(3.25) = 0.512, where the ratio is 2.113 and its
// base-ten logarithm is 0.325 -- a tenth of the decibel figure, as the definition requires. Rather than pick one
// reading, the family is refused and names the conversion.
//
// expect: fail
// expect-match: cannot take a logarithm or exponential of a decibel value
// expect-match: dimensionless(gain)
// forbid-match-gcc: candidate
#include <units/power.h>
using namespace units;
int main()
{
	auto bad = units::log10(units::decibels<double>(3.25)); // ill-formed
	(void)bad;
	return 0;
}
