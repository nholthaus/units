// Case: an operation on a quantity measured from an ARBITRARY ORIGIN whose answer depends on where that origin was
// put. Each formerly compiled and returned an origin-dependent number -- celsius(20)/celsius(10) is 2, while the
// same two temperatures in kelvin give 1.035, and -celsius(20) is a different temperature from the negation of
// 293.15 K. The remedy is the same operation on DIFFERENCES, which carry no origin.
//
// expect: fail
// expect-match: cannot negate a reading
// expect-match: negate a difference
// expect-match: celsius<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/temperature.h>
using namespace units;
int main()
{
	auto bad = -units::temperature::celsius<double>(20.5); // ill-formed
	(void)bad;
	return 0;
}
