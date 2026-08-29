// Case: fabs of a reading. abs is covered separately; both must be refused, since the magnitude of a datum-relative value depends on the datum.
//
// expect: fail
// expect-match: no origin-free magnitude
// expect-match: take fabs of a difference
// expect-match: celsius<
// forbid-match-gcc: candidate
#include <units/temperature.h>
using namespace units;
int main()
{
	auto bad = units::fabs(units::temperature::celsius<double>(-5.25)); (void)bad; // ill-formed
	return 0;
}
