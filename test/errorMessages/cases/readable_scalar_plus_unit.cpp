// Case: adding a bare double to a dimensioned quantity must FAIL readably, naming the unit type.
//
// COMPILER-CONTROLLED text: the diagnostic is the compiler's own no-matching-`operator+` / invalid-operands wording,
// not a library sentence, so it is graded by tight readable tokens — the FRIENDLY `meters<` type AND the failing
// `operator+` context (both surfaced on GCC-15 and clang-19) — plus anti-soup guards confirming the message does not
// descend into conversion_factor / dimension_t template internals.
//
// expect: fail
// expect-match: meters<
// expect-match: operator+
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
