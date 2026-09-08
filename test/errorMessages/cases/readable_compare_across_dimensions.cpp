// Case: comparing a length with a mass must FAIL readably. The relational-operator diagnostic reports the
// operands through their conversion-factor tag inside the `unit<...>` base (e.g. `unit<units::meters_>`), so
// it does not spell the friendly `meters<double>` form on any compiler. The readability signal asserted here
// is the failing operator name. Its SPELLING differs by compiler — g++/clang write it tight (`operator<`),
// MSVC inserts a space (`operator <`) — so it is asserted per-compiler. A bare `meters` stem is rejected: it
// would silently match the `meters_` tag in the base, defeating the check. Readability is verified two-sided:
// the operator IS named AND the message is not buried in conversion-factor / dimension soup (both forbid tokens
// confirmed absent on GCC-13/15, clang, and MSVC).
//
// grades: compiler
// expect: fail
// expect-match-gcc: operator<
// expect-match-msvc: operator <
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/mass.h>
using namespace units;
using namespace units::literals;
bool bad = (1.0_m < 1.0_kg); // ill-formed: incomparable dimensions
int main()
{
	(void)bad;
	return 0;
}
