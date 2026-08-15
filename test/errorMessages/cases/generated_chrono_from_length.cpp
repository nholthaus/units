// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: meters
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/length.h>
#include <chrono>
using namespace units;
using namespace units::literals;
std::chrono::seconds s = 1.0_m;
int main() { return 0; }
