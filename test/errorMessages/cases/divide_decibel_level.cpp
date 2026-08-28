// Case: dividing a decibel value in place is ill-formed for the same reason as scaling one — the dB number and the
// linear ratio behind it do not divide alike. Same silent-wrong-value history as scale_decibel_level.
//
// expect: fail
// expect-match: cannot divide a decibel value
// expect-match: dBW<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/power.h>
using namespace units;
int main()
{
	units::power::dBW<double> level(12.5);
	level /= 2.0; // ill-formed: cannot divide a decibel value
	return 0;
}
