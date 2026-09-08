// Case: `a /= b` where b is a quantity would change a's dimension (metres over seconds is a velocity), which cannot
// be stored back in a. The diagnostic names the remedy: compute the quotient by value.
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
#include <units/time.h>
using namespace units;
int main()
{
	units::length::meters<double> m(5.0);
	m /= units::time::seconds<double>(2.0); // ill-formed: the quotient is a velocity
	return 0;
}
