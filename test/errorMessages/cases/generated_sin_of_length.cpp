// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: meters<double>
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/angle.h>
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto x = sin(1.0_m);
int main() { return 0; }
