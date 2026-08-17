// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension comparison — the diagnostic must name
// the failing operator, never the raw conversion_factor<...> / dimension_t<...> soup.
// expect: fail
// expect-match-gcc: operator>
// expect-match-msvc: operator >
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/time.h>
using namespace units;
using namespace units::literals;
bool b = (1.0_m > 1.0_s);
int main() { return 0; }
