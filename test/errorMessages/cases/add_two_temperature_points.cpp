// Case: summing two affine POINTS is undefined (what is "300 K + 50 K" as a point?), so `kelvin + kelvin` must be
// ill-formed. Only point - point (-> delta), point +/- delta, and delta +/- delta are defined; to move a point by a
// relative amount, add a `delta` to it. The diagnostic is the compiler's own no-matching-`operator+` wording, so it
// is graded by tight readable tokens — the FRIENDLY `kelvin<` type AND the failing operator context — plus the
// anti-soup guards.
//
// expect: fail
// expect-match: kelvin<
// expect-match-gcc: operator+
// expect-match-msvc: operator +
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/temperature.h>
using namespace units;
auto bad = units::temperature::kelvin<double>(300.0) + units::temperature::kelvin<double>(50.0); // ill-formed: point + point
int main()
{
	(void)bad;
	return 0;
}
