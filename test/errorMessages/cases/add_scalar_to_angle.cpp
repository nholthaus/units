// Case: a bare number added to a named angle in place is meaningless (2.0 of what?), so `radians += double` must
// be ill-formed. The diagnostic is the compiler's own no-matching-`operator+=` wording, so it is graded by tight
// readable tokens — the FRIENDLY `radians<` type AND the failing operator context — plus the anti-soup guards.
// To turn by a relative amount, add an angle: `r += 2.0_rad` (or wrap the amount in a `delta` of the unit).
//
// expect: fail
// expect-match: cannot add a bare number to a quantity
// expect-match: radians<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/angle.h>
using namespace units;
int main()
{
	units::angle::radians<double> r(1.0);
	r += 2.0; // ill-formed: cannot add a bare number to an angle
	return 0;
}
