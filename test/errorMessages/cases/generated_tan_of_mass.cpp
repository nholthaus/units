// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> / dimension_t<...> soup.
// expect: fail
// expect-match: kilograms<
// forbid-match-gcc: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match-gcc: dimension_t<
#include <units/angle.h>
#include <units/mass.h>
using namespace units;
using namespace units::literals;
auto x = tan(units::mass::kilograms<double>(1.0));
int main() { return 0; }
