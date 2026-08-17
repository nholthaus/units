// Case: a point divided by a point is meaningless (no ratio of two absolute positions). The library emits a
// readable message rather than an overload-resolution wall.
//
// expect: fail
// expect-match: a point cannot be divided by a point
// forbid-match: conversion_factor<std::ratio
#include <units/kind.h>
#include <units/length.h>
using namespace units;
using namespace units::length;
auto bad = absolute<meters<double>>(4.0) / absolute<meters<double>>(2.0); // ill-formed: point / point
int main()
{
	(void)bad;
	return 0;
}
