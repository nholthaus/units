// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> / dimension_t<...> soup.
// expect: fail
// expect-match: coulombs<
// expect-match: amperes<
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/charge.h>
#include <units/current.h>
using namespace units;
using namespace units::literals;
units::current::amperes<double> x = units::charge::coulombs<double>(1.0);
int main() { return 0; }
