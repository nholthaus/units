// Case: the by-value spelling of dividing a reading by a number.
//
// expect: fail
// expect-match: cannot divide an affine point
// expect-match: difference of two readings
// expect-match: celsius<
// forbid-match-gcc: candidate
#include <units/temperature.h>
using namespace units;
int main()
{
	auto bad = units::temperature::celsius<double>(20.5) / 2.0; (void)bad; // ill-formed
	return 0;
}
