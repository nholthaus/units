// Case: scaling a reading by a dimensionless QUANTITY with the quantity on the left. A ratio-scaled operand is used because it has a friendly strong name.
//
// expect: fail
// expect-match: cannot scale an affine point
// expect-match: difference of two readings
// expect-match: celsius<
// forbid-match-gcc: candidate
#include <units/temperature.h>
#include <units/concentration.h>
using namespace units;
int main()
{
	auto bad = units::concentration::percent<double>(50.0) * units::temperature::celsius<double>(20.5); (void)bad; // ill-formed
	return 0;
}
