// Case: mixing a logarithmic operand with a linear one of the same dimension was the one decibel refusal that arrived
// as a bare unsatisfied constraint with no remedy named. `fdim(decibels(3), percent(2))` answered 19.9897 before,
// computing a difference that the subtraction of those two operands itself refuses. Two logarithmic operands are fine
// -- their difference is a gain -- so only the mixed pair is refused here. The linear operand is spelled `percent`
// rather than `dimensionless` because the latter is an alias for a raw `unit<...>`, which the compiler prints in full.
//
// expect: fail
// expect-match: cannot take the positive difference of a decibel value and a linear one
// expect-match: dimensionless(gain)
// forbid-match: conversion_factor<std::ratio
#include <units/concentration.h>
#include <units/core.h>
using namespace units;
int main()
{
	auto bad = units::fdim(units::decibels<double>(3.0), units::percent<double>(2.0)); // ill-formed
	(void)bad;
	return 0;
}
