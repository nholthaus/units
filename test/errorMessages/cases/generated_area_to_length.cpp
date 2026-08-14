// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: square_meters<double>
// expect-match: meters<double>
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/area.h>
#include <units/length.h>
using namespace units;
using namespace units::literals;
units::length::meters<double> x = units::area::square_meters<double>(4.0);
int main() { return 0; }
