// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> / dimension_t<...> soup.
// expect: fail
// expect-match: cubic_meters<
// expect-match: square_meters<
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/volume.h>
#include <units/area.h>
using namespace units;
using namespace units::literals;
units::area::square_meters<double> x = units::volume::cubic_meters<double>(1.0);
int main() { return 0; }
