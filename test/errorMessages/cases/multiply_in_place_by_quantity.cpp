// Case: `a *= b` where b is a quantity would change a's dimension (metres times metres is an area), which cannot be
// stored back in a. The diagnostic names the remedy: compute the product by value.
//
// expect: fail
// expect-match: cannot multiply in place by a quantity
// expect-match: auto p = a * b
// forbid-match: conversion_factor<std::ratio
// forbid-match-gcc: candidate
#include <units/length.h>
using namespace units;
int main()
{
	units::length::meters<double> m(5.0);
	m *= units::length::meters<double>(2.0); // ill-formed: the product is an area
	return 0;
}
