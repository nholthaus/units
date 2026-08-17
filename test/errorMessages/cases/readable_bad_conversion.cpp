// Case: a deliberate bad conversion (length -> frequency) must FAIL with a READABLE message
// that names the friendly types `meters<...>` and `hertz<...>`, NOT the raw conversion_factor<...> soup.
// This is exactly what traits::strong<> exists to protect; any strong<> refactor must keep it.
// The `<`-suffixed tokens match the friendly template form (`meters<double>` on g++, the default-elided
// `meters<>` on clang/MSVC) while rejecting the conversion-factor tag `meters_` and the plain `unit<...>`
// base, so a soup regression fails the assertion.
//
// EXPECT: compile FAILS, and the error text names `meters<` and `hertz<`
//         (the readability assertion; see the harness's expect: block below).
//
// expect: fail
// expect-match: meters<
// expect-match: hertz<
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
