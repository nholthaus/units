// Case: adding two absolute decibel LEVELS (dBW + dBW / dBW + dBm) is a point + point and is meaningless — two
// 10 dBW sources are not a 20 dBW source. The addition operator for two dimensioned, same-dimension decibel
// operands is deleted, so the expression is ill-formed. The defined operations are level + gain -> level,
// gain + gain -> gain, and level - level -> gain; to combine independent power levels, add them in the linear
// domain (two equal powers sum to +3 dB), not by adding their dB numbers.
//
// The diagnostic names the deleted operator and the concrete dBW operand type, and does not fall back to raw
// conversion_factor / dimension soup. `dBW` is the friendly operand name and survives on every compiler, so it
// is asserted universally; the operator SPELLING differs (g++/clang write it tight, `operator+`, MSVC inserts a
// space, `operator +`), so it is asserted per-compiler. Readability is verified two-sided: the friendly name and
// operator ARE present AND the message is not buried in soup (both forbid tokens confirmed absent on all four).
//
// grades: compiler
// expect: fail
// expect-match: dBW
// expect-match-gcc: operator+
// expect-match-msvc: operator +
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/power.h>
using namespace units;
using namespace units::power;
auto bad = dBW<double>(10.0) + dBW<double>(10.0); // ill-formed: absolute level + absolute level
int main()
{
	(void)bad;
	return 0;
}
