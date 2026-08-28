// Case: a bare number added to a length in place is meaningless (3.0 of what?), so `meters += double` must be
// ill-formed. The diagnostic is the compiler's own no-matching-`operator+=` wording, so it is graded by tight
// readable tokens — the FRIENDLY `meters<` type AND the failing operator context — plus the anti-soup guards.
// To move a length by a relative amount, add a length: `m += 3.0_m` (or wrap the amount in a `delta` of the unit).
//
// expect: fail
// expect-match: meters<
// expect-match-gcc: operator+=
// expect-match-msvc: operator +=
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/length.h>
using namespace units;
int main()
{
	units::length::meters<double> m(5.0);
	m += 3.0; // ill-formed: cannot add a bare number to a length
	return 0;
}
