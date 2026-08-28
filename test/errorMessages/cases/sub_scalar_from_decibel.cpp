// Case: mirror of add_scalar_to_decibel -- a bare number states neither a reference nor a ratio, so it cannot be
// subtracted from a decibel gain. Must be refused at the CALL SITE, naming a decibels(...) gain.
//
// expect: fail
// expect-match: cannot subtract a bare number from a decibel value
// expect-match: decibels(
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/power.h>
using namespace units;
int main()
{
	units::decibels<double> gain(3.5);
	gain -= 2.25; // ill-formed
	return 0;
}
