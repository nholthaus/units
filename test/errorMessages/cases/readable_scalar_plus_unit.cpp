// Case: adding a bare double to a dimensioned quantity must FAIL readably, naming the unit type.
//
// COMPILER-CONTROLLED text: the diagnostic is the compiler's own no-matching-`operator+` / invalid-operands wording,
// not a library sentence, so it is graded by tight readable tokens — the FRIENDLY `meters<` type AND the failing
// operator context — plus anti-soup guards. g++/clang spell the operator tight (`operator+`), MSVC inserts a space
// (`operator +`), so the operator token is per-compiler. The anti-soup markers are confirmed absent on GCC-15,
// clang-19, and MSVC.
//
// expect: fail
// expect-match: meters<
// expect-match-gcc: operator+
// expect-match-msvc: operator +
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto bad = 1.0_m + 5.0; // ill-formed: cannot add a raw scalar to a length
int main()
{
	(void)bad;
	return 0;
}
