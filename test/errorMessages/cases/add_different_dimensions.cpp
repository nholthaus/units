// Case: compound-adding quantities of different dimensions is forbidden by dimensional analysis. The diagnostic
// says so in one line instead of listing declined candidates.
//
// Deletion, not a body `static_assert`, and a per-compiler line bound: see README.md "Deleted overloads".
//
// expect: fail
// grades: compiler
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
