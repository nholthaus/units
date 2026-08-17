// Case: assigning an area (m^2) to a length (m) must FAIL readably, naming both friendly types.
//
// COMPILER-CONTROLLED text: the diagnostic is the compiler's own no-viable-conversion wording (g++ "conversion from
// ... to non-scalar type ... requested", clang "no viable conversion from ... to"), so it is graded by tight readable
// tokens — both FRIENDLY strong types (each surfaced through the compiler's `(aka ...)` / `{aka ...}` clarifier) AND
// the `conversion` context — not a verbatim sentence. The `conversion_factor<...>` marker is NOT forbidden here: on
// clang the derived result's type is spelled through `squared<...conversion_factor>` and the length.h `UNIT_ADD`
// macro note legitimately echoes `conversion_factor<std::ratio<1>, dimension::length>`, so forbidding it would be a
// false regression signal. The `dimension_t<` dimensionless-soup marker IS forbidden (absent on GCC-15 and clang-19).
//
// expect: fail
// expect-match: square_meters<
// expect-match: meters<
// expect-match: conversion
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/area.h>
using namespace units;
using namespace units::literals;
units::length::meters<double> a = 1.0_m * 1.0_m; // ill-formed: m*m is an area, not a length
int main()
{
	(void)a;
	return 0;
}
