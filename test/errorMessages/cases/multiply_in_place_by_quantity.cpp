// Case: `a *= b` where b is a quantity would change a's dimension (metres times metres is an area), which cannot be
// stored back in a. The diagnostic names the remedy: compute the product by value.
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
