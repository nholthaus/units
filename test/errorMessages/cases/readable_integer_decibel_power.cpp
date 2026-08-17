// Case: a power decibel unit (dBW) with an integral underlying type must FAIL readably, for the same
// reason as the dimensionless decibel — a logarithmic scale cannot use an integer underlying type.
//
// LIBRARY-CONTROLLED text: the near-verbatim static_assert sentence units emits (core.h) is asserted — including the
// parenthetical reason — so a reword regresses the test. Unlike the dimensionless `decibels<int>` case, a `dBW<int>`
// carries the `watts_` conversion factor, so its instantiation backtrace does NOT descend into the
// `conversion_factor<std::ratio<1>, dimension_t<>>` dimensionless form; the anti-soup guards therefore hold and are
// asserted absent (confirmed on GCC-15 and clang-19).
//
// expect: fail
// expect-match: a decibel-scale unit requires a floating-point underlying type (an integral type cannot represent a logarithmic value)
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/power.h>
using namespace units;
units::power::dBW<int> a(10); // ill-formed: integral underlying type on a decibel scale
int main()
{
	(void)a;
	return 0;
}
