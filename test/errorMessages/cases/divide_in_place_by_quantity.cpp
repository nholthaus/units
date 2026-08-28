// Case: `a /= b` where b is a quantity would change a's dimension (metres over seconds is a velocity), which cannot
// be stored back in a. The diagnostic names the remedy: compute the quotient by value.
//
// expect: fail
// expect-match: cannot divide in place by a quantity
// expect-match: auto q = a / b
// forbid-match: conversion_factor<std::ratio
#include <units/length.h>
#include <units/time.h>
using namespace units;
int main()
{
	units::length::meters<double> m(5.0);
	m /= units::time::seconds<double>(2.0); // ill-formed: the quotient is a velocity
	return 0;
}
