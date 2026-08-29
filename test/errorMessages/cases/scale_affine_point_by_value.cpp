// Case: the by-value spelling of scaling a reading. The compound form is covered separately; this is the one that produced celsius c = celsius(20) * 2.0 -> -233.15 degC, and the one an Eigen scalar product reaches.
//
// expect: fail
// expect-match: cannot scale an affine point
// expect-match: difference of two readings
// expect-match: celsius<
// forbid-match-gcc: candidate
#include <units/temperature.h>
using namespace units;
int main()
{
	auto bad = units::temperature::celsius<double>(20.5) * 2.0; (void)bad; // ill-formed
	return 0;
}
