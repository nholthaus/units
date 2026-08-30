// Case: a decibel value is a LOGARITHMIC reading, and the dB number does not scale like the linear ratio it stands
// for (doubling the dB number squares the ratio), so `dBW *= double` must be ill-formed. Computing it would read the
// value THROUGH the scale (`raw()` is the dB number) and write it back PAST the scale, giving 13.98 dBW for
// dBW(12.5) *= 2.0 -- neither reading. Scale the linear quantity instead.
//
// expect: fail
// expect-match: cannot scale a decibel value
// expect-match: dBW<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/power.h>
using namespace units;
int main()
{
	units::power::dBW<double> level(12.5);
	level *= 2.0; // ill-formed
	return 0;
}
