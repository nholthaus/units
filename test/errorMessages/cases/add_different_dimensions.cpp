// Case: compound-adding quantities of different dimensions is forbidden by dimensional analysis. The diagnostic
// says so in one line instead of listing declined candidates.
//
// The refusal is expressed by DELETING the overload rather than by a `static_assert` in its body. A body-fired
// assertion resolves the overload, so a `requires`-expression reports the operation as available and generic
// code with a SFINAE fallback hard-errors from inside the library instead of taking its fallback. Deletion is
// observable, at the cost of the remedy sentence, so what is graded is that the diagnostic is short and names
// both operand types.
//
// NOTE the per-compiler bound: for a DELETED overload gcc prints the declaration and stops, while clang lists
// every declined candidate. Deletion is still the right mechanism -- a body-fired `static_assert` resolves the
// overload and makes an invalid operation look available to a `requires`-expression -- but on clang it costs
// the terseness it buys on gcc, so the bound records what each compiler actually does.
//
// expect: fail
// expect-match: deleted
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// expect-max-lines: 25
// expect-max-lines-clang: 115
#include <units/length.h>
#include <units/temperature.h>
using namespace units;
int main()
{
	units::temperature::celsius<double> c(20.0);
	c += units::length::meters<double>(1.0); // ill-formed: a length is not a temperature
	return 0;
}
