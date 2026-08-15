// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: meters<double>
// expect-match: seconds<double>
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/area.h>
#include <units/length.h>
#include <units/time.h>
using namespace units;
using namespace units::literals;
units::time::seconds<double> x = sqrt(units::area::square_meters<double>(4.0));
int main() { return 0; }
