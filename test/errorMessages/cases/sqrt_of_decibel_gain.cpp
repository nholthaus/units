// Case: a root of a logarithm is not the root of the ratio it denotes. sqrt(decibels(3.25)) read 1.802776, the square
// root of 3.25, where the ratio is 2.113489 and its root is 1.453441.
//
// expect: fail
// expect-match: cannot apply sqrt to a decibel value
// expect-match: dimensionless(gain)
// forbid-match: conversion_factor<std::ratio
#include <units/core.h>
using namespace units;
int main()
{
	auto bad = units::sqrt(units::decibels<double>(3.25)); // ill-formed
	(void)bad;
	return 0;
}
