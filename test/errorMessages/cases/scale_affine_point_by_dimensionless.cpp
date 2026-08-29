// Case: `celsius * percent(50)` means exactly what `celsius * 2.0` means, so it must be refused identically. Written
// with a dimensionless QUANTITY it took a different overload that had no affine exclusion, so it quietly scaled the
// point (40 degC for `* dimensionless(2)`) while the bare-number spelling was refused -- which made the rule
// decorative. A ratio-dimensionless operand is used here because it has a friendly strong name; a plain
// `dimensionless` operand is refused by the same overload but prints its own conversion factor, having no name.
//
// expect: fail
// expect-match: cannot scale an affine point
// expect-match: units::delta<
// expect-match: celsius<
// expect-match: percent<
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
// forbid-match-gcc: candidate
#include <units/concentration.h>
#include <units/temperature.h>
using namespace units;
int main()
{
	auto bad = units::temperature::celsius<double>(20.0) * units::concentration::percent<double>(50.0); // ill-formed
	(void)bad;
	return 0;
}
