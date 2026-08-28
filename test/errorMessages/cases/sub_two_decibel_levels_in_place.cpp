// Case: `level -= level` cannot work -- the difference of two decibel levels is a dimensionless GAIN, which cannot be
// stored back in the level's own type. It formerly failed INSIDE the library, at the assignment that could not hold
// the gain, so the reader was shown a library line rather than a remedy. Compute it by value instead.
//
// expect: fail
// expect-match: cannot subtract two decibel levels in place
// expect-match: dBW<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/power.h>
using namespace units;
int main()
{
	units::power::dBW<double> level(12.5);
	level -= units::power::dBW<double>(3.0); // ill-formed
	return 0;
}
