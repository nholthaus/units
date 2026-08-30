// Case: the inverse trigonometric and hyperbolic functions read a quantity's value on the same terms as the
// logarithmic ones, so a decibel scale is refused for them too. `atan(decibels(3.25))` reading 3.25 gives
// 1.2723 rad where the ratio the gain denotes, 2.113, gives 1.1288 -- a wrong answer rather than a refusal.
//
// expect: fail
// expect-match: cannot apply atan to a decibel value
// expect-match: dimensionless(gain)
// forbid-match-gcc: candidate
#include <units/angle.h>
using namespace units;
int main()
{
	auto bad = units::atan(units::decibels<double>(3.25)); // ill-formed
	(void)bad;
	return 0;
}
