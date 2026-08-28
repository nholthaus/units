// Case: kelvin is an affine-dimension POINT too (a temperature reading), even without a per-unit datum offset, so
// scaling it is meaningless ("2 x 300 K" scales the reading, not an amount). `kelvin *= double` must be ill-formed.
// The diagnostic is the compiler's own no-matching-`operator*=` wording, graded by tight readable tokens — the
// FRIENDLY `kelvin<` type AND the failing operator context — plus the anti-soup guards. Only a `delta` (an amount)
// scales: `delta<kelvin<double>>(300.0) * 2.0`.
//
// expect: fail
// expect-match: kelvin<
// expect-match-gcc: operator*=
// expect-match-msvc: operator *=
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/temperature.h>
using namespace units;
int main()
{
	units::temperature::kelvin<double> k(300.0);
	k *= 2.0; // ill-formed: cannot scale an affine point (a temperature reading)
	return 0;
}
