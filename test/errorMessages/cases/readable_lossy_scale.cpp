// Case: scaling an integer-backed unit by a floating-point factor is lossy. It is intentionally a WARNING,
// not a hard error (the operation still compiles and truncates), and the diagnostic must read cleanly: it
// names the friendly unit type and its integer underlying, not the raw conversion_factor<ratio, dimension>
// soup. The `meters<` token matches the friendly template form (`meters<int>`) while rejecting the
// conversion-factor tag `meters_` and the plain `unit<...>` base; `int` proves the integer underlying.
//
// expect: pass
// flags: -Wconversion -Wfloat-conversion
// flags-msvc: /W4
// expect-match: meters<
// expect-match: int
// forbid-match: conversion_factor<std::ratio
#include <units/length.h>
using namespace units::literals;
void scale()
{
	units::length::meters<int> m(3);
	m *= 1.5; // ill-formed
}
int main()
{
	scale();
	return 0;
}
