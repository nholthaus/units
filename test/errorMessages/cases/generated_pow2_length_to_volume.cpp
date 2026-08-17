// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> / dimension_t<...> soup.
// expect: fail
// expect-match: square_meters<
// expect-match: cubic_meters<
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/area.h>
#include <units/volume.h>
using namespace units;
using namespace units::literals;
units::volume::cubic_meters<double> x = pow<2>(1.0_m);
int main() { return 0; }
