// Case: assigning a compound (derived) result to the wrong named unit must FAIL readably.
// velocity = length / time; assigning it to acceleration_ names both strong types.
//
// COMPILER-CONTROLLED text: the diagnostic is the compiler's own no-viable-conversion wording, so it is graded by
// tight readable tokens — the FRIENDLY source (`meters_per_second<`) AND destination (`meters_per_second_squared<`)
// strong types (each surfaced through the compiler's `(aka ...)` / `{aka ...}` clarifier) AND the `conversion`
// context — not a verbatim sentence. As with the area→length case, `conversion_factor<...>` is NOT forbidden (the
// derived type is spelled through `compound_conversion_factor<...>` on clang), but the `dimension_t<`
// dimensionless-soup marker IS forbidden (absent on GCC-15 and clang-19).
//
// expect: fail
// expect-match: meters_per_second<
// expect-match: meters_per_second_squared<
// expect-match: conversion
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>
#include <units/acceleration.h>
using namespace units::literals;
units::acceleration::meters_per_second_squared<double> a = 1.0_m / 1.0_s; // ill-formed: velocity -> acceleration
int main()
{
	(void)a;
	return 0;
}
