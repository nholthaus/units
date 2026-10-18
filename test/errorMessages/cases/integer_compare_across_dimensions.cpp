// Integral comparisons must reject differing dimensions even without constructing a common unit.
// expect: fail
#include <units/length.h>
#include <units/time.h>
bool bad = units::meters<int>(1) < units::seconds<int>(1);
int  main()
{
	return bad;
}
