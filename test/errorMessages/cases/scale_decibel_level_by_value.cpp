// Case: the by-value `dBW * 2.0` has no valid overload -- every arithmetic `*` requires a linear scale on both
// operands -- so without a diagnostic the compiler prints 149 lines and 12 declined candidates, where the compound
// `dBW *= 2.0` reports one sentence. The two forms are equally ill-formed and equally wrong to write, so they carry
// the same message. Doubling a dB number squares the ratio it denotes; scale the linear quantity instead.
//
// expect: fail
// expect-match: cannot scale a decibel value
// expect-match: watts(level)
// forbid-match-gcc: candidate
#include <units/power.h>
using namespace units;
int main()
{
	auto bad = units::dBW<double>(12.5) * 2.0; // ill-formed
	(void)bad;
	return 0;
}
