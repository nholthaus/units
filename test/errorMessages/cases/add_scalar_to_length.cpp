// Case: a bare number added to a length in place is meaningless (3.0 of what?), so `meters += double` must be
// ill-formed. The diagnostic is the library's own message, graded on a phrase only it can emit, plus tight readable tokens — the FRIENDLY `meters<` type AND the failing operator context — plus the anti-soup guards.
// To move a length by a relative amount, add a length: `m += 3.0_m` (or wrap the amount in a `delta` of the unit).
//
// The refusal is expressed by DELETING the overload rather than by a `static_assert` in its body. A body-fired
// assertion resolves the overload, so a `requires`-expression reports the operation as available and generic
// code with a SFINAE fallback hard-errors from inside the library instead of taking its fallback. Deletion is
// observable, at the cost of the remedy sentence, so what is graded is that the diagnostic is short and names
// both operand types.
//
// NOTE the per-compiler bound: for a DELETED overload gcc prints the declaration and stops, while clang lists
// every declined candidate. Deletion is still the right mechanism -- a body-fired `static_assert` resolves the
// overload and makes an invalid operation look available to a `requires`-expression -- but on clang it costs
// the terseness it buys on gcc, so the bound records what each compiler actually does.
//
// expect: fail
// expect-match: deleted
// expect-match: meters<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// expect-max-lines: 25
// expect-max-lines-clang: 115
#include <units/length.h>
using namespace units;
int main()
{
	units::length::meters<double> m(5.0);
	m += 3.0; // ill-formed: cannot add a bare number to a length
	return 0;
}
