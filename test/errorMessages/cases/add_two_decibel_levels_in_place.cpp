// Case: `level += level` cannot work -- adding two logarithmic levels has no meaning (two 10 dBW sources are not a
// 20 dBW source), so the by-value `operator+` is deleted. Without this diagnostic the compound form reports the
// deleted function from inside the library rather than naming what to do: combine the powers in the linear domain.
//
// expect: fail
// expect-match: cannot add two decibel levels
// expect-match: dBW<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/power.h>
using namespace units;
int main()
{
	units::power::dBW<double> level(12.5);
	level += units::power::dBW<double>(3.0); // ill-formed
	return 0;
}
