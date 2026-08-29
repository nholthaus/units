// Case: dividing an affine POINT by a bare number is meaningless (its value is datum-relative — "20 degC / 2"
// depends on the datum), so `celsius /= double` must be ill-formed. The diagnostic is the library's own message, graded on a phrase only it can emit — the FRIENDLY `celsius<` type AND the failing
// operator context — plus the anti-soup guards. Only a `delta` (an amount) divides: `delta<celsius<double>>(20.0) / 2.0`.
//
// expect: fail
// expect-match: cannot divide an affine point
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
	c /= 2.0; // ill-formed
	return 0;
}
