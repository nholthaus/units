// Case: a bare number subtracted from a length in place is meaningless (3.0 of what?), so `meters -= double` must
// be ill-formed. The diagnostic is the library's own message, graded on a phrase only it can emit, plus tight readable tokens — the FRIENDLY `meters<` type AND the failing operator context — plus the anti-soup guards.
// To shorten by a relative amount, subtract a length: `m -= 3.0_m` (or wrap the amount in a `delta` of the unit).
//
// expect: fail
// expect-match: cannot subtract a bare number from a quantity
// expect-match: meters<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/length.h>
using namespace units;
int main()
{
	units::length::meters<double> m(5.0);
	m -= 3.0; // ill-formed: cannot subtract a bare number from a length
	return 0;
}
