// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: meters_per_second<double>
// expect-match: kilograms<double>
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>
#include <units/mass.h>
using namespace units;
using namespace units::literals;
units::mass::kilograms<double> x = 10.0_m / 2.0_s;
int main() { return 0; }
