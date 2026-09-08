// Case: `a *= b` where b is a quantity would change a's dimension (metres times metres is an area), which cannot be
// stored back in a. The diagnostic names the remedy: compute the product by value.
//
// Deletion, not a body `static_assert`, and a per-compiler line bound: see README.md "Deleted overloads".
//
// expect: fail
// grades: compiler
// expect-match: deleted
// forbid-match: conversion_factor<std::ratio
// expect-max-lines: 25
// expect-max-lines-clang: 115
#include <units/length.h>
using namespace units;
int main()
{
	units::length::meters<double> m(5.0);
	m *= units::length::meters<double>(2.0); // ill-formed: the product is an area
	return 0;
}
