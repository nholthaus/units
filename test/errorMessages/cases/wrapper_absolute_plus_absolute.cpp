// Case: summing two affine POINTS is meaningless (what is "20 degC + 5 degC" as a point?), so
// `absolute<U> + absolute<V>` must be ill-formed. Only point - point (-> delta), point +/- delta,
// and delta +/- delta are defined; to move a point by a relative amount, add a `delta` to it.
//
// The diagnostic names the friendly `absolute`/`celsius<double>` wrapper types and the operator under
// consideration. g++/clang spell it tight (`operator+`), MSVC inserts a space (`operator +`), so the operator
// token is per-compiler. The message does not fall back to the raw conversion_factor<std::ratio<...>> soup.
//
// expect: fail
// expect-match-gcc: operator+
// expect-match-msvc: operator +
// expect-match: absolute
// expect-match: celsius<double>
// forbid-match: conversion_factor<std::ratio
#include <units/kind.h>
#include <units/temperature.h>
using namespace units;
using namespace units::temperature;
auto bad = absolute<celsius<double>>(20.0) + absolute<celsius<double>>(5.0); // ill-formed: point + point
int main()
{
	(void)bad;
	return 0;
}
