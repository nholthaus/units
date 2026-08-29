// Case: sqrt of a reading. sqrt(celsius(16)) gave 4 from the celsius reading; the same temperature in kelvin gives 17.
//
// expect: fail
// expect-match: no origin-free root
// expect-match: take sqrt of a difference
// expect-match: celsius<
// forbid-match-gcc: candidate
#include <units/temperature.h>
using namespace units;
int main()
{
	auto bad = units::sqrt(units::temperature::celsius<double>(16.0)); (void)bad; // ill-formed
	return 0;
}
