// any_unit::try_to<T>() rejects a bare float target with a friendly message.
//
// LIBRARY-CONTROLLED text: the near-verbatim static_assert sentence units emits (serialization.h) is asserted so a
// reword regresses the test. A non-unit target produces ONLY the friendly message, never conversion_factor /
// dimension_t soup (confirmed on GCC-15 and clang-19).
//
// expect: fail
// expect-match: any_unit::try_to<T>() collapses into a unit type (e.g. meters<double>), not a bare number.
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/serialization.h>
using namespace units::literals;
auto make()
{
	auto v = units::deserialize(units::serialize(1.0_m));
	return v->try_to<float>();
}
int main() { (void)make; return 0; }
