// Case: summing two affine POINTS is meaningless (what is "20 degC + 5 degC" as a point?), so
// `absolute<U> + absolute<V>` must be ill-formed. Only point - point (-> delta), point +/- delta,
// and delta +/- delta are defined. The diagnostic must name the friendly absolute/celsius wrapper
// types, never the raw conversion_factor<...> soup.
//
// The harness (run.py) normalizes the match directives to be portable across g++, clang, and MSVC
// (it strips class/struct/enum keywords and collapses whitespace), so one directive covers every
// compiler; the harness has no separate per-compiler directive.
//
// expect: fail
// expect-match: operator+
// expect-match: absolute
// expect-match: celsius<double>
// forbid-match: conversion_factor<std::ratio
#include <units/temperature.h>
using namespace units;
using namespace units::temperature;
auto bad = absolute<celsius<>>(celsius<>(20.0)) + absolute<celsius<>>(celsius<>(5.0)); // ill-formed: point + point
int main()
{
	(void)bad;
	return 0;
}
