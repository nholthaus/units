// Case: compound-adding quantities of different dimensions is forbidden by dimensional analysis. The diagnostic
// says so in one line instead of listing declined candidates.
//
// expect: fail
// expect-match: cannot add quantities of different dimensions
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/temperature.h>
using namespace units;
int main()
{
	units::temperature::celsius<double> c(20.0);
	c += units::length::meters<double>(1.0); // ill-formed: a length is not a temperature
	return 0;
}
