// Case: copysign with a UNIT as the sign source (the bare-number overload is covered separately).
//
// expect: fail
// expect-match: no origin-free sign
// expect-match: copysign a difference
// expect-match: celsius<
// forbid-match-gcc: candidate
#include <units/temperature.h>
using namespace units;
int main()
{
	auto bad = units::copysign(units::temperature::celsius<double>(5.25), units::temperature::celsius<double>(-1.0)); (void)bad; // ill-formed
	return 0;
}
