// Case: comparing a length with a mass must FAIL readably. The relational-operator diagnostic reports the
// operands through their conversion-factor tag inside the `unit<...>` base (e.g. `unit<units::meters_>`), so
// it does not spell the friendly `meters<double>` form on any compiler. The readability signal asserted here
// is the failing operator name, `operator<`, which is portable across g++ and clang and is not soup. A bare
// `meters` stem is rejected: it would silently match the `meters_` tag in the base, defeating the check.
//
// expect: fail
// expect-match: operator<
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
