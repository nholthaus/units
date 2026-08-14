// Case: comparing a length with a mass must FAIL readably, naming the friendly types.
//
// expect: fail
// expect-match: meters
// expect-match: kilograms
#include <units/length.h>
#include <units/mass.h>
using namespace units;
using namespace units::literals;
bool bad = (1.0_m < 1.0_kg); // ill-formed: incomparable dimensions
int main()
{
	(void)bad;
	return 0;
}
