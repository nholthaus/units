// Case: a bare number added to an affine POINT in place is meaningless (a Celsius reading is datum-relative, so
// "+ 5.0" of what?), so `celsius += double` must be ill-formed. The diagnostic is the compiler's own
// no-matching-`operator+=` wording, graded by tight readable tokens — the FRIENDLY `celsius<` type AND the failing
// operator context — plus the anti-soup guards. To move the point by a relative amount, add a `delta`:
// `c += delta<celsius<double>>(5.0)`.
//
// expect: fail
// expect-match: cannot add a bare number to an affine point
// expect-match: units::delta<
// expect-match: celsius<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/temperature.h>
using namespace units;
int main()
{
	units::temperature::celsius<double> c(20.0);
	c += 5.0; // ill-formed: cannot add a bare number to an affine point
	return 0;
}
