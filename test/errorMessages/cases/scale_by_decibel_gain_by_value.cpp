// Case: applying a gain to a power by multiplying is the classic link-budget mistake. A dB gain is a logarithmic
// figure, so `watts * decibels(3.0)` would multiply by 3.0 where the ratio is 1.995. The by-value form carries the
// same sentence as the compound `watts *= decibels(3.0)`, and names the conversion that makes it a plain factor.
//
// expect: fail
// expect-match: cannot scale by a decibel gain
// expect-match: dimensionless(gain)
// forbid-match-gcc: candidate
#include <units/power.h>
using namespace units;
int main()
{
	auto bad = units::watts<double>(4.0) * units::decibels<double>(3.0); // ill-formed
	(void)bad;
	return 0;
}
