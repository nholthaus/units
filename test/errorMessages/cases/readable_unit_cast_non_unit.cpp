// unit_cast<T>(any_unit) rejects a bare arithmetic target with a friendly message.
//
// LIBRARY-CONTROLLED text: the near-verbatim static_assert sentence units emits (serialization.h) is asserted so a
// reword regresses the test. A non-unit target produces ONLY the friendly message, never conversion_factor /
// dimension_t soup (confirmed on GCC-15 and clang-19).
//
// expect: fail
// expect-match: units::unit_cast<T>(any_unit) casts to a unit type (e.g. meters<double>), not a bare number. Collapse to a unit, then read its value.
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/serialization.h>
using namespace units::literals;
auto make()
{
	auto v = units::deserialize(units::serialize(1.0_m));
	return units::unit_cast<long>(*v);
}
int main() { (void)make; return 0; }
