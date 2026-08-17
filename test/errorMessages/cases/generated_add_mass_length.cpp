// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: kilograms<
// expect-match: meters<
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/mass.h>
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto bad = 1.0_kg + 1.0_m;
int main() { return 0; }
