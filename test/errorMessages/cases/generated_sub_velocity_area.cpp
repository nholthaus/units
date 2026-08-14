// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: meters_per_second<double>
// expect-match: square_meters<double>
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/velocity.h>
#include <units/area.h>
using namespace units;
using namespace units::literals;
auto bad = 1.0_mps - units::area::square_meters<double>(1.0);
int main() { return 0; }
