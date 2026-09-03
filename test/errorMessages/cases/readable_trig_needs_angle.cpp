// Case: calling sin() on a length must FAIL readably; trigonometric functions require an angle.
//
// COMPILER-CONTROLLED text: the units `sin` overload is SFINAE-gated to a dimensionless argument, so a length falls
// through to a no-matching-function / cannot-convert diagnostic that is the compiler's own wording (g++ "cannot
// convert ... to 'double'", clang "no matching function for call to 'sin'"). It is graded by tight readable tokens:
// the FRIENDLY `meters<` type AND the `sin` call context (both named in stable diagnostic text on GCC-15 and
// clang-19), plus anti-soup guards confirming no conversion_factor / dimension_t template internals.
//
// grades: compiler
// expect: fail
// NOTE: do not grade on `sin` -- it appears in the echoed source line, so the case would grade itself.
// expect-match: meters<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto bad = sin(1.0_m); // ill-formed
int main()
{
	(void)bad;
	return 0;
}
