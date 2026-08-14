// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: meters_per_second<double>
// expect-match: meters<double>
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/velocity.h>
#include <units/length.h>
using namespace units;
using namespace units::literals;
units::length::meters<double> x = 1.0_mps;
int main() { return 0; }
