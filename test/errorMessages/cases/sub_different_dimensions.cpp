// Case: mirror of add_different_dimensions -- compound subtraction across different dimensions is forbidden by
// dimensional analysis, and the message must say so rather than printing a candidate wall.
//
// Deletion, not a body `static_assert`, and a per-compiler line bound: see README.md "Deleted overloads".
//
// expect: fail
// grades: compiler
// expect-match: deleted
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// expect-max-lines: 25
// expect-max-lines-clang: 115
#include <units/length.h>
#include <units/time.h>
using namespace units;
int main()
{
	units::meters<double> m(5.0);
	m -= units::time::seconds<double>(2.0); // ill-formed
	return 0;
}
