// Case: a bare number subtracted from a length in place is meaningless (3.0 of what?), so `meters -= double` must
// be ill-formed. The diagnostic is the library's own message, graded on a phrase only it can emit, plus tight readable tokens — the FRIENDLY `meters<` type AND the failing operator context — plus the anti-soup guards.
// To shorten by a relative amount, subtract a length: `m -= 3.0_m` (or wrap the amount in a `delta` of the unit).
//
// The refusal is expressed by DELETING the overload, matching its `+=` twin. A body-fired `static_assert`
// resolves the overload, so a `requires`-expression reports the operation as available and generic code with a
// SFINAE fallback hard-errors from inside the library. Deletion is observable, at the cost of the remedy
// sentence, so what is graded is that the diagnostic is short and names the operand types.
//
// expect: fail
// expect-match-gcc: use of deleted function
// expect-match-msvc: deleted function
// expect-match: meters<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// expect-max-lines: 25
#include <units/length.h>
using namespace units;
int main()
{
	units::length::meters<double> m(5.0);
	m -= 3.0; // ill-formed: cannot subtract a bare number from a length
	return 0;
}
