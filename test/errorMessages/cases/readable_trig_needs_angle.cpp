// Case: calling sin() on a length must FAIL readably; trigonometric functions require an angle.
//
// expect: fail
// expect-match: meters<double>
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto bad = sin(1.0_m); // ill-formed: sin expects an angle, not a length
int main()
{
	(void)bad;
	return 0;
}
