// Case: a point cannot be scaled — "2 x the 3 o'clock mark" is meaningless. Only a delta (an amount) scales.
// `absolute<U> * scalar` must be ill-formed.
//
// expect: fail
// expect-match: meters
// forbid-match: conversion_factor<std::ratio
#include <units/kind.h>
#include <units/length.h>
using namespace units;
using namespace units::length;
auto bad = absolute<meters<double>>(2.0) * 2.0; // ill-formed: scaling a point
int main()
{
	(void)bad;
	return 0;
}
