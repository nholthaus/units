// Case: `absolute<>` marks a quantity as a POINT so that a point and an amount cannot be confused. A decibel type
// already carries that distinction in its dimension -- a LEVEL (dBW, dBm) is dimensioned and a GAIN (decibels) is
// dimensionless -- so the wrapper adds no information, and its scaling and magnitude operations have no single
// reading of a logarithm to work from. Refused with the plain types named as the remedy.
//
// expect: fail
// expect-match: a decibel quantity cannot be wrapped
// forbid-match: conversion_factor<std::ratio
#include <units/kind.h>
#include <units/power.h>
using namespace units;
int main()
{
	units::absolute<units::power::dBW<double>> bad(3.0); // ill-formed
	(void)bad;
	return 0;
}
