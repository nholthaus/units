// Case: the `delta<>` half of the same rule. A decibel GAIN is already the change of a LEVEL, so wrapping either in
// `delta<>` states nothing the type did not, and the wrapper's own magnitude arithmetic has no single reading of a
// logarithm to work from.
//
// expect: fail
// expect-match: a decibel quantity cannot be wrapped
// forbid-match: conversion_factor<std::ratio
#include <units/kind.h>
#include <units/power.h>
using namespace units;
int main()
{
	units::delta<units::power::dBW<double>> bad(3.0); // ill-formed
	(void)bad;
	return 0;
}
