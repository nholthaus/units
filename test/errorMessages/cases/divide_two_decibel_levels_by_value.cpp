// Case: the ratio of two same-dimension decibel values is their DIFFERENCE in dB -- 20 dBW over 10 dBW is a 10 dB
// gain, not the 2 that dividing the dB numbers gives. This is the one shape of decibel division with a direct
// remedy, so it has its own message naming `a - b` rather than the generic "use the linear values".
//
// expect: fail
// expect-match: cannot divide two decibel values of one dimension
// expect-match: a - b
// forbid-match-gcc: candidate
#include <units/power.h>
using namespace units;
int main()
{
	auto bad = units::dBW<double>(20.0) / units::dBW<double>(10.0); // ill-formed
	(void)bad;
	return 0;
}
