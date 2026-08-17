// Case: adding a bare double to a dimensioned quantity must FAIL readably, naming the unit type.
//
// expect: fail
// expect-match: meters<
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto bad = 1.0_m + 5.0; // ill-formed: cannot add a raw scalar to a length
int main()
{
	(void)bad;
	return 0;
}
