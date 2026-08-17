// any_unit::to<T>() rejects a std::ratio target with a friendly message.
//
// LIBRARY-CONTROLLED text: the near-verbatim static_assert sentence units emits (serialization.h) is asserted so a
// reword regresses the test. The body is `if constexpr`-gated, so a non-unit target produces ONLY the friendly
// message, never conversion_factor / dimension_t soup (confirmed on GCC-15 and clang-19).
//
// expect: fail
// expect-match: any_unit::to<T>() collapses into a unit type (e.g. meters<double>), not a bare number. To read a plain value, collapse to a unit first, then call .value() or .to<double>() on that unit.
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <ratio>
#include <units/length.h>
#include <units/serialization.h>
using namespace units::literals;
auto make()
{
	auto v = units::deserialize(units::serialize(1.0_m));
	return v->to<std::ratio<1>>();
}
int main() { (void)make; return 0; }
