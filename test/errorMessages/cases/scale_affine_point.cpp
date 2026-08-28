// Case: scaling an affine POINT is meaningless (its value is datum-relative — "2 x 20 degC" depends on the datum),
// so `celsius *= double` must be ill-formed. The diagnostic is the compiler's own no-matching-`operator*=` wording,
// graded by tight readable tokens — the FRIENDLY `celsius<` type AND the failing operator context — plus the
// anti-soup guards. Only a `delta` (an amount) scales: `delta<celsius<double>>(20.0) * 2.0`.
//
// expect: fail
// expect-match: celsius<
// expect-match-gcc: operator*=
// expect-match-msvc: operator *=
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/temperature.h>
using namespace units;
int main()
{
	units::temperature::celsius<double> c(20.0);
	c *= 2.0; // ill-formed: cannot scale an affine point
	return 0;
}
