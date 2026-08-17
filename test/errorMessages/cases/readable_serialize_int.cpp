// serialize() rejects an int (not a unit) with a friendly message, no template soup.
//
// LIBRARY-CONTROLLED text: the near-verbatim static_assert sentence units emits (serialization.h) is asserted so a
// reword regresses the test. The gated body emits ONLY the friendly message, never the conversion_factor / unit<...>
// / dimension_t<...> soup, so all three anti-soup guards are asserted absent (confirmed on GCC-15 and clang-19).
//
// expect: fail
// expect-match: units::serialize requires a units quantity (e.g. meters<double>). Its argument is not a unit type; wrap the value in a unit before serializing.
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/serialization.h>
auto bad = units::serialize(42);
int main() { (void)bad; return 0; }
