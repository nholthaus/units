// Case: adding a bare double to a dimensioned quantity must FAIL readably, naming the unit type.
//
// A selected diagnostic overload replaces the compiler's candidate list with one library sentence naming the remedy,
// and the `dependent_false<Lhs, T>` note still names the FRIENDLY `meters<double>` type. Without it the compiler
// reports its own no-matching-`operator+` wording followed by every declined overload -- 116 lines and 11 candidates
// on 3.6.1. A DIMENSIONLESS quantity does add a bare number, so only a dimensioned left operand is matched. Graded
// on: the sentence, the type name, and the failing operator context (g++/clang `operator+`, MSVC `operator +`), plus
// the anti-soup and candidate guards.
//
// expect: fail
// expect-match: cannot add a bare number to a quantity
// expect-match: meters<
// expect-match-gcc: operator+
// expect-match-msvc: operator +
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto bad = 1.0_m + 5.0; // ill-formed: cannot add a raw scalar to a length
int main()
{
	(void)bad;
	return 0;
}
