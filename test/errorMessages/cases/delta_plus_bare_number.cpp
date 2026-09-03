// Case: adding a bare number to a delta is ill-formed (a delta adds a delta). The naive fumble gets a readable
// message, not a candidate wall. Wrap the number in a delta of the same unit.
//
// expect: fail
// expect-match: cannot add a bare number to a delta
// forbid-match: conversion_factor<std::ratio
// forbid-match-gcc: candidate
#include <units/kind.h>
#include <units/length.h>
using namespace units;
using namespace units::length;
auto bad = delta<meters<double>>(5.0) + 3.0; // ill-formed: delta + bare number
int main()
{
	(void)bad;
	return 0;
}
