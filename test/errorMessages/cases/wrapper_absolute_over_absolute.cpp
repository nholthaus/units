// Case: a point divided by a point is meaningless — there is no ratio of two absolute positions.
// `absolute<U> / absolute<V>` must be ill-formed.
//
// expect: fail
// expect-match: meters
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
