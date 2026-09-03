// Case: compound-adding quantities of different dimensions is forbidden by dimensional analysis. The diagnostic
// says so in one line instead of listing declined candidates.
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
// forbid-match: dimension_t<
// expect-max-lines: 25
#include <units/length.h>
#include <units/temperature.h>
using namespace units;
int main()
{
	units::temperature::celsius<double> c(20.0);
	c += units::length::meters<double>(1.0); // ill-formed: a length is not a temperature
	return 0;
}
