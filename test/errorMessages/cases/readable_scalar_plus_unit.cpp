// Case: adding a bare double to a dimensioned quantity must FAIL readably, naming the unit type.
//
// Without a diagnostic the compiler
// reports its own no-matching-`operator+` wording followed by every declined overload -- 116 lines and 11 candidates
// on 3.6.1. A dimensionless quantity does add a bare number, so only a dimensioned left operand is matched. Graded
// on: the sentence, the type name, and the failing operator context (g++/clang `operator+`, MSVC `operator +`), plus
// the anti-soup and candidate guards.
//
// Deletion, not a body `static_assert`, and a per-compiler line bound: see README.md "Deleted overloads".
//
// expect: fail
// expect-match: deleted
// expect-match: meters<
// expect-match-gcc: operator+
// expect-match-msvc: operator +
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// expect-max-lines: 25
// expect-max-lines-clang: 115
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto bad = 1.0_m + 5.0; // ill-formed: cannot add a raw scalar to a length
int main()
{
	(void)bad;
	return 0;
}
