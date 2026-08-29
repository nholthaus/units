// Case: a bare number added to a decibel GAIN states neither a reference nor a ratio, so `decibels += double` must be
// ill-formed and must say so at the CALL SITE. A dimensionless dB gain is an ordinary dimensionless unit, so it
// formerly reached the plain-scalar overload and failed inside the library ("no match for operator+") instead. Add a
// `decibels(...)` gain.
//
// expect: fail
// expect-match: cannot add a bare number to a decibel value
// expect-match: decibels(
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/power.h>
using namespace units;
int main()
{
	units::decibels<double> gain(3.5);
	gain += 2.25; // ill-formed
	return 0;
}
