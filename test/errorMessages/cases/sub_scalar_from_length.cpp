// Case: a bare number subtracted from a length in place is meaningless (3.0 of what?), so `meters -= double` must
// be ill-formed. The diagnostic is the library's own message, graded on a phrase only it can emit, plus tight readable tokens — the friendly `meters<` type and the failing operator context — plus the anti-soup guards.
// To shorten by a relative amount, subtract a length: `m -= 3.0_m` (or wrap the amount in a `delta` of the unit).
//
// Deletion, not a body `static_assert`, and a per-compiler line bound: see README.md "Deleted overloads".
//
// expect: fail
// grades: compiler
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
	m -= 3.0; // ill-formed: cannot subtract a bare number from a length
	return 0;
}
