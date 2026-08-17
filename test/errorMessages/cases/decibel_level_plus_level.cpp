// Case: adding two absolute decibel LEVELS (dBW + dBW / dBW + dBm) is a point + point and is meaningless — two
// 10 dBW sources are not a 20 dBW source. The addition operator for two dimensioned, same-dimension decibel
// operands is deleted, so the expression is ill-formed. The defined operations are level + gain -> level,
// gain + gain -> gain, and level - level -> gain; to combine independent power levels, add them in the linear
// domain (two equal powers sum to +3 dB), not by adding their dB numbers.
//
// The diagnostic names the deleted operator+ and the concrete dBW operand type (portable across compilers: the
// operator name and `dBW` survive g++/clang/MSVC spelling differences), and does not fall back to raw
// conversion_factor soup.
//
// expect: fail
// expect-match: operator+
// expect-match: dBW
// forbid-match: conversion_factor<std::ratio
#include <units/power.h>
using namespace units;
using namespace units::power;
auto bad = dBW<double>(10.0) + dBW<double>(10.0); // ill-formed: absolute level + absolute level
int main()
{
	(void)bad;
	return 0;
}
