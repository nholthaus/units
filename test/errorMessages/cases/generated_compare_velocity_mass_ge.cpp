// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
// expect: fail
// expect-match: operator>=
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
#include <units/velocity.h>
#include <units/mass.h>
using namespace units;
using namespace units::literals;
bool b = (1.0_mps >= units::mass::kilograms<double>(1.0));
int main() { return 0; }
