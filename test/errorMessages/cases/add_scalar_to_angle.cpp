// Case: a bare number added to a named angle in place is meaningless (2.0 of what?), so `radians += double` must
// be ill-formed. The diagnostic is the library's own message, graded on a phrase only it can emit, plus tight readable tokens — the friendly `radians<` type and the failing operator context — plus the anti-soup guards.
// To turn by a relative amount, add an angle: `r += 2.0_rad` (or wrap the amount in a `delta` of the unit).
//
// Deletion, not a body `static_assert`, and a per-compiler line bound: see README.md "Deleted overloads".
//
// expect: fail
// grades: compiler
// expect-match: deleted
// expect-match: radians<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// expect-max-lines: 25
// expect-max-lines-clang: 115
#include <units/angle.h>
using namespace units;
int main()
{
	units::angle::radians<double> r(1.0);
	r += 2.0; // ill-formed: cannot add a bare number to an angle
	return 0;
}
