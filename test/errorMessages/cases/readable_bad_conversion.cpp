// Case: a deliberate bad conversion (length -> frequency) must FAIL with a READABLE message
// that names the strong types `meters_` and `hertz_`, NOT the raw conversion_factor<...> soup.
// This is exactly what traits::strong<> exists to protect; any strong<> refactor must keep it.
//
// EXPECT: compile FAILS, and the error text contains "meters_" and "hertz_"
//         (the readability assertion; see the harness's expect: block below).
//
// expect: fail
// expect-match: meters
// expect-match: hertz
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/length.h>
#include <units/frequency.h>
using namespace units::literals;
units::frequency::hertz<double> f = 1.0_m; // ill-formed on purpose
int main()
{
	(void)f;
	return 0;
}
