// Case: the sum of two temperature READINGS has no meaning -- a reading is a point on a scale with a datum, and
// adding two points is undefined. The diagnostic must say so and name the remedy (subtract them for a change, or
// move one with `+=`), not print a wall of declined candidates.
//
// expect: fail
// expect-match: cannot add two affine points
// expect-match: subtract them for an amount
// expect-match: celsius<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/temperature.h>
using namespace units;
int main()
{
	auto bad = units::temperature::celsius<double>(20.0) + units::temperature::fahrenheit<double>(68.0);
	(void)bad; // ill-formed: the sum of two readings has no meaning
	return 0;
}
