// Case: mirror of add_scalar_to_affine -- a bare number carries no unit, so it cannot state how much to cool a
// reading by. `celsius -= 5.0` must be refused and must name a quantity or a delta as the remedy.
//
// expect: fail
// expect-match: cannot subtract a bare number from an affine point
// expect-match: subtract a quantity of the same dimension
// expect-match: celsius<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/temperature.h>
using namespace units;
int main()
{
	units::temperature::celsius<double> c(20.0);
	c -= 5.0; // ill-formed: a bare number states no amount of change
	return 0;
}
