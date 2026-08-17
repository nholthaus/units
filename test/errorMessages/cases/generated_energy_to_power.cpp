// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> / dimension_t<...> soup.
// expect: fail
// expect-match: joules<
// expect-match: watts<
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/energy.h>
#include <units/power.h>
using namespace units;
using namespace units::literals;
units::power::watts<double> x = units::energy::joules<double>(3.0);
int main() { return 0; }
