// Case: scaling a reading with the number on the LEFT -- the same operation, and it must be refused the same way.
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
	auto bad = 2.0 * units::temperature::celsius<double>(20.5); (void)bad; // ill-formed
	return 0;
}
