// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> / dimension_t<...> soup.
// expect: fail
// expect-match: hertz<
// expect-match: radians<
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/frequency.h>
#include <units/angle.h>
using namespace units;
using namespace units::literals;
auto bad = units::frequency::hertz<double>(1.0) + units::angle::radians<double>(1.0);
int main() { return 0; }
