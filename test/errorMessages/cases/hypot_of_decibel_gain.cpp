// Case: `hypot` takes two operands, so the unary family macro cannot declare its diagnostic and the refusal arrived
// as a bare overload-resolution failure. A hypotenuse of two logarithms is not the hypotenuse of the ratios they
// denote: hypot(decibels(3.25), decibels(2.0)) read 3.816084 from the dB figures.
//
// expect: fail
// expect-match: cannot take the hypotenuse of a decibel value
// expect-match: dimensionless(gain)
// forbid-match: conversion_factor<std::ratio
#include <units/core.h>
using namespace units;
int main()
{
	auto bad = units::hypot(units::decibels<double>(3.25), units::decibels<double>(2.0)); // ill-formed
	(void)bad;
	return 0;
}
