// Case: the PRODUCT of two quantities where either is measured from an arbitrary origin has no origin-free answer.
// celsius(20)*celsius(30) gave 600 from the celsius readings and 89,610 from the same two temperatures in kelvin, and
// celsius(20)*meters(2) gave "40 m K". This hole is what let an Eigen dot product and norm over a matrix of readings
// compile and return a datum-dependent number. The remedy is to multiply DIFFERENCES, which carry no origin.
//
// expect: fail
// expect-match: no origin-free product
// expect-match: multiply their differences
// expect-match: celsius<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/temperature.h>
using namespace units;
int main()
{
	auto bad = units::temperature::celsius<double>(20.5) * units::temperature::celsius<double>(30.5); // ill-formed
	(void)bad;
	return 0;
}
