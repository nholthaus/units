// Case: summing two affine POINTS is meaningless (what is "20 degC + 5 degC" as a point?), so
// `absolute<U> + absolute<V>` must be ill-formed. Only point - point (-> delta), point +/- delta,
// and delta +/- delta are defined; to move a point by a relative amount, add a `delta` to it.
//
// The library emits a readable message (a static_assert in a catch-all overload) naming the mistake, rather than
// an overload-resolution wall. The message is asserted near-verbatim; it fires on g++, clang, and MSVC.
//
// expect: fail
// expect-match: cannot add two points
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
