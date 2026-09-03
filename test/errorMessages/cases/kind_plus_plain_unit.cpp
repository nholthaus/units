// Case: mixing a `kind` with a PLAIN unit in arithmetic is ill-formed — a plain unit carries no kind, so combining
// it with a tagged quantity would silently launder an untagged value into the kind. A plain unit becomes a kind
// only by deliberate construction/assignment, never mid-expression. The library gives a readable, remedy-naming
// diagnostic (a static_assert catch-all) instead of a "no matching operator" wall.
//
// Library-controlled message, asserted near-verbatim; the static_assert fires on every compiler.
//
// expect: fail
// expect-match: cannot add a plain unit to a kind
// expect-match: carries no kind
// forbid-match: conversion_factor<std::ratio
// forbid-match-gcc: candidate
#include <units/kind.h>
#include <units/length.h>
using namespace units;
using namespace units::length;
auto bad = kind<"radial", meters<double>>(5.0) + meters<double>(3.0); // kind + plain unit
int main()
{
	(void)bad;
	return 0;
}
