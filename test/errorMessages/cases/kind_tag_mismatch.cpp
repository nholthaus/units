// Case: two DIFFERENT kinds (a radial distance and a straight-line distance) share a unit and a dimension but are
// semantically distinct, so combining them must be ill-formed. The library provides a readable, tag-naming
// diagnostic (a static_assert in a least-preferred catch-all overload) rather than an overload-resolution wall.
//
// The library controls this message, so it is asserted near-verbatim: it names both the concept ("kind") and the
// remedy. The `dependent_false` static_assert fires on every compiler (a plain static_assert message, not a
// consteval throw), so one `expect-match` covers g++, clang, and MSVC.
//
// expect: fail
// expect-match: cannot add two DIFFERENT kinds
// expect-match: radial
// expect-match: straight
// forbid-match: conversion_factor<std::ratio
#include <units/kind.h>
#include <units/length.h>
using namespace units;
using namespace units::length;
auto bad = kind<"radial", meters<double>>(5.0) + kind<"straight", meters<double>>(3.0); // different kinds
int main()
{
	(void)bad;
	return 0;
}
