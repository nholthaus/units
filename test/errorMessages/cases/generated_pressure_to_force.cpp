// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: pascals<
// expect-match: newtons<
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/pressure.h>
#include <units/force.h>
using namespace units;
using namespace units::literals;
units::force::newtons<double> x = units::pressure::pascals<double>(5.0);
int main() { return 0; }
