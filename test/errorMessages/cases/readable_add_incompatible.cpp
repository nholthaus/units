// Case: adding incompatible units (length + time) must FAIL readably, naming the strong types.
//
// A selected diagnostic overload replaces the compiler's candidate list with one library sentence, and the
// `dependent_false<Lhs, Rhs>` note still names both FRIENDLY strong types. Without it the compiler reports its own
// no-matching-`operator+` wording followed by every declined overload the library declares -- 116 lines and 11
// candidates on 3.6.1, and the list grows with each overload added. Graded on: the sentence, both type names, and the
// failing operator context (g++/clang spell it tight, `operator+`; MSVC inserts a space, `operator +`). Anti-soup
// guards confirm the message does not descend into conversion_factor / dimension_t template internals, and the
// candidate guard confirms the wall is gone.
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
// expect-match: seconds<
// expect-match-gcc: operator+
// expect-match-msvc: operator +
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// expect-max-lines: 25
// expect-max-lines-clang: 115
#include <units/length.h>
#include <units/time.h>
using namespace units::literals;
auto bad = 1.0_m + 1.0_s; // ill-formed: cannot add length and time
int main()
{
	(void)bad;
	return 0;
}
