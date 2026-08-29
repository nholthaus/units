// Case: `abs` on a quantity measured from an ARBITRARY ORIGIN -- an affine reading (a datum) or a decibel LEVEL (a
// logarithmic reference) -- has no origin-free answer, so it must be refused. It formerly compiled and returned an
// origin-dependent number: the same physical quantity written in a different scale gave a different result, e.g.
// abs of -5.25 degC is 5.25 degC = 278.4 K while abs of the identical 267.9 K is 267.9 K. The remedy is the same
// function on a DIFFERENCE, which carries no origin.
//
// expect: fail
// expect-match: no origin-free magnitude
// expect-match: take abs of a difference
// expect-match: dBW<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/power.h>
using namespace units;
int main()
{
	auto bad = units::abs(units::power::dBW<double>(-3.25)); // ill-formed
	(void)bad;
	return 0;
}
