// Case: the hyperbolic functions take a dimensionless argument; calling cosh() on a length must FAIL
// readably, naming the offending strong type and the dimensionless constraint it did not satisfy.
//
// COMPILER-CONTROLLED text: the units `cosh` overload takes a dimensionless argument, so a length yields a
// no-matching-function diagnostic in the compiler's own wording. It is graded by tight readable tokens: the FRIENDLY
// `meters<` type, the `dimensionless` domain the candidate names, AND the `cosh` call context (all named on GCC-15,
// clang-19, and MSVC). The anti-soup guards are GCC/clang-only: MSVC prints the full `units::cosh(dimensionlessUnit)`
// candidate signature in an overload-resolution note, which legitimately carries the dimensionless
// `conversion_factor<std::ratio<1,1>, dimension_t<>>` even though the friendly argument type is also named — so the
// guards are scoped to the compilers where that soup would be a genuine regression.
//
// expect: fail
// expect-match: meters<
// expect-match: dimensionless
// expect-match: cosh
// forbid-match-gcc: conversion_factor<std::ratio
// forbid-match-gcc: dimension_t<
#include <units/angle.h>
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto bad = cosh(1.0_m); // ill-formed
int main()
{
	(void)bad;
	return 0;
}
