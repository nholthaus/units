// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> / dimension_t<...> soup.
// expect: fail
// expect-match: hertz<
// expect-match: seconds<
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/frequency.h>
#include <units/time.h>
using namespace units;
using namespace units::literals;
units::time::seconds<double> x = units::frequency::hertz<double>(2.0);
int main() { return 0; }
