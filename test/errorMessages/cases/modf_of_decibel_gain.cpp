// Case: `modf` takes a pointer to the integral part, so the unary UNIT_ADD_LOGARITHMIC_SCALE_DIAGNOSTIC macro cannot
// declare its diagnostic either. Splitting a logarithm into integral and fractional parts does not split the ratio it
// denotes: modf(decibels(3.25)) answered a fraction of 0.25 against an integral part of 4.7712, which is the
// LINEARIZED value's integer part -- two numbers from different domains.
//
// expect: fail
// expect-match: cannot split a decibel value into integral and fractional parts
// expect-match: modf(dimensionless(gain)
// forbid-match: conversion_factor<std::ratio
#include <units/core.h>
using namespace units;
int main()
{
	units::decibels<double> part(0.0);
	auto bad = units::modf(units::decibels<double>(3.25), &part); // ill-formed
	(void)bad;
	return 0;
}
