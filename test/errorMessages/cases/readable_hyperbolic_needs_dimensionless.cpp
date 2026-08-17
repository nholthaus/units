// Case: the hyperbolic functions take a dimensionless argument; calling cosh() on a length must FAIL
// readably, naming the offending strong type and the dimensionless constraint it did not satisfy.
//
// COMPILER-CONTROLLED text: the units `cosh` overload is SFINAE-gated on `is_dimensionless_unit_v`, so a length
// yields a no-matching-function diagnostic in the compiler's own wording. It is graded by tight readable tokens: the
// FRIENDLY `meters<` type, the `dimensionless` constraint the candidate did not satisfy, AND the `cosh` call context
// (all named on GCC-15 and clang-19), plus anti-soup guards confirming no conversion_factor / dimension_t internals.
//
// expect: fail
// expect-match: meters<
// expect-match: dimensionless
// expect-match: cosh
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/angle.h>
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto bad = cosh(1.0_m); // ill-formed: cosh expects a dimensionless value, not a length
int main()
{
	(void)bad;
	return 0;
}
