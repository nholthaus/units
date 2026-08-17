// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> / dimension_t<...> soup.
// expect: fail
// expect-match: meters<
// expect-match: seconds<
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/time.h>
using namespace units;
using namespace units::literals;
auto x = fmod(1.0_m, 1.0_s);
int main() { return 0; }
