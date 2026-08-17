// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: kelvin<
// expect-match: seconds<
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/temperature.h>
#include <units/time.h>
using namespace units;
using namespace units::literals;
units::time::seconds<double> x = units::temperature::kelvin<double>(300.0);
int main() { return 0; }
