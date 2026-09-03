// Case: `atan2` is the one member of the transcendental family taking two arguments, so the unary
// UNIT_ADD_LOGARITHMIC_SCALE_DIAGNOSTIC macro cannot declare it and it needs its own overload. Without one the
// constrained `atan2` withdraws itself and the C library's `::atan2` claims the call through a dimensionless
// quantity's conversion to double, answering with the decibel FIGURES: atan2(decibels(3), decibels(2)) gives
// 0.98279 where the ratios 1.995 and 1.585 give 0.89952. A wrong answer rather than a refusal.
//
// expect: fail
// expect-match: cannot apply atan2 to a decibel value
// expect-match: dimensionless(gain)
// forbid-match: conversion_factor<std::ratio
#include <units/angle.h>
using namespace units;
int main()
{
	auto bad = units::atan2(units::decibels<double>(3.0), units::decibels<double>(2.0)); // ill-formed
	(void)bad;
	return 0;
}
