// Case: a bare number added to a named angle in place is meaningless (2.0 of what?), so `radians += double` must
// be ill-formed. The diagnostic is the library's own message, graded on a phrase only it can emit, plus tight readable tokens — the FRIENDLY `radians<` type AND the failing operator context — plus the anti-soup guards.
// To turn by a relative amount, add an angle: `r += 2.0_rad` (or wrap the amount in a `delta` of the unit).
//
// The refusal is expressed by DELETING the overload rather than by a `static_assert` in its body. A body-fired
// assertion resolves the overload, so a `requires`-expression reports the operation as available and generic
// code with a SFINAE fallback hard-errors from inside the library instead of taking its fallback. Deletion is
// observable, at the cost of the remedy sentence, so what is graded is that the diagnostic is short and names
// both operand types.
//
// expect: fail
// expect-match-gcc: use of deleted function
// expect-match-msvc: deleted function
// expect-match: radians<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// expect-max-lines: 25
#include <units/angle.h>
using namespace units;
int main()
{
	units::angle::radians<double> r(1.0);
	r += 2.0; // ill-formed: cannot add a bare number to an angle
	return 0;
}
