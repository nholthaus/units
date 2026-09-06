// Case: `sin`, `cos` and `tan` take an ANGLE, so a dimensionless decibel value never reaches the library's own
// overload and the C library's `::sin` claims the call through the conversion to `double` -- answering from the dB
// FIGURE. sin(decibels(3.25)) read -0.108195, the sine of 3.25, where the ratio 3.25 dB denotes is 2.113489 and its
// sine is 0.856321. A wrong answer rather than a refusal, and the same shape as the rest of the family.
//
// expect: fail
// expect-match: cannot apply sin to a decibel value
// expect-match: dimensionless(gain)
// forbid-match: conversion_factor<std::ratio
#include <units/angle.h>
using namespace units;
int main()
{
	auto bad = sin(units::decibels<double>(3.25)); // ill-formed
	(void)bad;
	return 0;
}
