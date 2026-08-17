// Case: a point cannot be scaled ("2 x the 3 o'clock mark" is meaningless). Only a delta (an amount) scales.
// The library emits a readable message rather than an overload-resolution wall.
//
// expect: fail
// expect-match: a point cannot be scaled
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
