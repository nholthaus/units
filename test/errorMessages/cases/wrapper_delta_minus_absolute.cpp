// Case: a vector minus a point is meaningless — only point - point (-> delta) and point - delta (-> point)
// are defined, never delta - absolute. Must be ill-formed.
//
// Compiler-controlled rejection (no viable operator-), graded by the friendly wrapper/unit type tokens and an
// anti-soup guard.
//
// expect: fail
// expect-match: celsius
// forbid-match: conversion_factor<std::ratio
#include <units/kind.h>
#include <units/temperature.h>
using namespace units;
using namespace units::temperature;
auto bad = delta<celsius<double>>(5.0) - absolute<celsius<double>>(20.0); // ill-formed: vector - point
int main()
{
	(void)bad;
	return 0;
}
