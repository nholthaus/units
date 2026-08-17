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
// COMPILER-CONTROLLED text: the diagnostic is the compiler's own no-viable-conversion wording, graded by tight
// readable tokens (both FRIENDLY strong types AND the `conversion` context) plus anti-soup guards. The existing
// dimensionless-soup marker is kept, and the broader `dimension_t<` marker is added — both confirmed absent on
// GCC-15 and clang-19. (The bare `conversion_factor<std::ratio` is NOT forbidden: clang spells the constructor
// candidates through `conversion_factor<std::ratio<1>, dimension::frequency>` etc., a legitimate context.)
//
// expect: fail
// expect-match: meters<
// expect-match: hertz<
// expect-match: conversion
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/frequency.h>
using namespace units::literals;
units::frequency::hertz<double> f = 1.0_m; // ill-formed on purpose
int main()
{
	(void)f;
	return 0;
}
