// Case: adding a bare double to a dimensioned quantity must FAIL readably, naming the unit type.
//
// A selected diagnostic overload replaces the compiler's candidate list with one library sentence naming the remedy,
// and the `dependent_false<Lhs, T>` note still names the FRIENDLY `meters<double>` type. Without it the compiler
// reports its own no-matching-`operator+` wording followed by every declined overload -- 116 lines and 11 candidates
// on 3.6.1. A DIMENSIONLESS quantity does add a bare number, so only a dimensioned left operand is matched. Graded
// on: the sentence, the type name, and the failing operator context (g++/clang `operator+`, MSVC `operator +`), plus
// the anti-soup and candidate guards.
//
// The refusal is expressed by DELETING the overload rather than by a `static_assert` in its body. A body-fired
// assertion resolves the overload, so a `requires`-expression reports the operation as available and generic
// code with a SFINAE fallback hard-errors from inside the library instead of taking its fallback. Deletion is
// observable, at the cost of the remedy sentence, so what is graded is that the diagnostic is short and names
// both operand types.
//
// grades: compiler
// NOTE the per-compiler bound: for a DELETED overload gcc prints the declaration and stops, while clang lists
// every declined candidate. Deletion is still the right mechanism -- a body-fired `static_assert` resolves the
// overload and makes an invalid operation look available to a `requires`-expression -- but on clang it costs
// the terseness it buys on gcc, so the bound records what each compiler actually does.
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
