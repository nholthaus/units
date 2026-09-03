// Case: `fmod` takes two operands, so the unary UNIT_ADD_LOGARITHMIC_SCALE_DIAGNOSTIC macro cannot declare its
// diagnostic and it needs its own overload. Without one the constrained `fmod` simply withdraws itself and the
// refusal arrives as a bare overload-resolution failure naming no remedy -- the only member of the decibel set
// reported that way. `dBW % dBW` was already refused, and a remainder of two dB FIGURES is not the remainder of
// the powers they denote: fmod(dBW(12.5), dBW(4.25)) answered 4.
//
// expect: fail
// expect-match: cannot take the remainder of a decibel value
// expect-match: Work in the linear domain
// forbid-match: conversion_factor<std::ratio
#include <units/power.h>
using namespace units;
int main()
{
	auto bad = units::fmod(units::power::dBW<double>(12.5), units::power::dBW<double>(4.25)); // ill-formed
	(void)bad;
	return 0;
}
