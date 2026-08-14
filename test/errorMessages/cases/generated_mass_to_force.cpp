// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: kilograms<double>
// expect-match: newtons<double>
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/mass.h>
#include <units/force.h>
using namespace units;
using namespace units::literals;
units::force::newtons<double> x = 1.0_kg;
int main() { return 0; }
