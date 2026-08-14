// Case: adding incompatible units (length + time) must FAIL readably, naming the strong types.
//
// expect: fail
// expect-match: meters
// expect-match: seconds
#include <units/length.h>
#include <units/time.h>
using namespace units::literals;
auto bad = 1.0_m + 1.0_s; // ill-formed: cannot add length and time
int main()
{
	(void)bad;
	return 0;
}
