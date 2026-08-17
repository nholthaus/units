// deserialize<T>(bytes) rejects a non-unit T with a friendly message directing to the erased form.
//
// LIBRARY-CONTROLLED text: the near-verbatim static_assert sentence units emits (serialization.h) is asserted so a
// reword regresses the test. A non-unit T produces ONLY the friendly message, never conversion_factor /
// dimension_t soup (confirmed on GCC-15 and clang-19).
//
// expect: fail
// expect-match: units::deserialize<T>(bytes) decodes into a unit type (e.g. deserialize<meters<double>>). The requested type is not a unit; use deserialize(bytes) for an erased any_unit.
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/serialization.h>
using namespace units::literals;
auto make() { return units::deserialize<double>(units::serialize(1.0_m)); }
int main() { (void)make; return 0; }
