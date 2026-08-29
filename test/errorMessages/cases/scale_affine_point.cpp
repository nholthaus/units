// Case: scaling an affine POINT is meaningless (its value is datum-relative — "2 x 20 degC" depends on the datum),
// so `celsius *= double` must be ill-formed. The diagnostic is the library's own message, graded on a phrase only it
// can emit plus the FRIENDLY `celsius<` type, with the anti-soup guards. What does scale is an AMOUNT: a difference of
// two readings (`celsius(20) - celsius(0)`) needs no wrapper, and `delta<celsius<double>>` states the intent in the
// type.
//
// expect: fail
// expect-match: cannot scale an affine point
// expect-match: units::delta<
// expect-match: celsius<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/temperature.h>
using namespace units;
int main()
{
	units::temperature::celsius<double> c(20.0);
	c *= 2.0; // ill-formed
	return 0;
}
