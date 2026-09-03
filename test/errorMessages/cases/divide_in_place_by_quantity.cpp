// Case: `a /= b` where b is a quantity would change a's dimension (metres over seconds is a velocity), which cannot
// be stored back in a. The diagnostic names the remedy: compute the quotient by value.
//
// The refusal is expressed by DELETING the overload rather than by a `static_assert` in its body. A body-fired
// assertion resolves the overload, so a `requires`-expression reports the operation as available and generic
// code with a SFINAE fallback hard-errors from inside the library instead of taking its fallback. Deletion is
// observable, at the cost of the remedy sentence, so what is graded is that the diagnostic is short and names
// both operand types.
//
// expect: fail
// expect-match-gcc: use of deleted function
// expect-match-msvc: deleted function
// forbid-match: conversion_factor<std::ratio
// expect-max-lines: 25
#include <units/length.h>
#include <units/time.h>
using namespace units;
int main()
{
	units::length::meters<double> m(5.0);
	m /= units::time::seconds<double>(2.0); // ill-formed: the quotient is a velocity
	return 0;
}
