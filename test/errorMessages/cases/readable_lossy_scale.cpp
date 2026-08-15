// Case: scaling an integer-backed unit by a floating-point factor is lossy. It is intentionally a WARNING,
// not a hard error (the operation still compiles and truncates), and the diagnostic must read cleanly: it
// names the friendly underlying integer type, not the raw conversion_factor<ratio, dimension> soup.
//
// expect: pass
// flags: -Wconversion -Wfloat-conversion
// flags-msvc: /W4
// expect-match: meters
// expect-match: int
// forbid-match: conversion_factor<std::ratio
#include <units/length.h>
using namespace units::literals;
void scale()
{
	units::length::meters<int> m(3);
	m *= 1.5; // lossy: warns (float -> int), does not fail
}
int main()
{
	scale();
	return 0;
}
