// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: square_meters<double>
// expect-match: seconds<double>
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/length.h>
#include <units/time.h>
#include <units/area.h>
using namespace units;
using namespace units::literals;
units::time::seconds<double> x = 2.0_m * 2.0_m;
int main() { return 0; }
