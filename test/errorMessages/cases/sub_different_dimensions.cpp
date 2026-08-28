// Case: mirror of add_different_dimensions -- compound subtraction across different dimensions is forbidden by
// dimensional analysis, and the message must say so rather than printing a candidate wall.
//
// expect: fail
// expect-match: cannot subtract quantities of different dimensions
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/time.h>
using namespace units;
int main()
{
	units::meters<double> m(5.0);
	m -= units::time::seconds<double>(2.0); // ill-formed
	return 0;
}
