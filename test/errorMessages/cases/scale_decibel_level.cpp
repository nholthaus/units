// Case: a decibel value is a LOGARITHMIC reading, and the dB number does not scale like the linear ratio it stands
// for (doubling the dB number squares the ratio), so `dBW *= double` must be ill-formed. It formerly COMPILED and
// produced a value that was neither reading: the body read the value through the scale (`raw()` is the dB number) and
// wrote it back past the scale, so dBW(12.5) *= 2.0 yielded 13.98 dBW. Scale the linear quantity instead.
//
// expect: fail
// expect-match: cannot scale a decibel value
// expect-match: dBW<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/power.h>
using namespace units;
int main()
{
	units::power::dBW<double> level(12.5);
	level *= 2.0; // ill-formed: cannot scale a decibel value
	return 0;
}
