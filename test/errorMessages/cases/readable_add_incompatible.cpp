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
// expect: fail
// expect-match: cannot add quantities of different dimensions
// expect-match: meters<
// expect-match: seconds<
// expect-match-gcc: operator+
// expect-match-msvc: operator +
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/length.h>
#include <units/time.h>
using namespace units::literals;
auto bad = 1.0_m + 1.0_s; // ill-formed: cannot add length and time
int main()
{
	(void)bad;
	return 0;
}
