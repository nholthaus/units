// any_unit::assign_to(out) rejects a bare int target with a friendly message; it assigns into a UNIT variable.
//
// LIBRARY-CONTROLLED text: the near-verbatim static_assert sentence units emits (serialization.h) is asserted so a
// reword regresses the test. A non-unit target produces ONLY the friendly message, never conversion_factor /
// dimension_t soup (confirmed on GCC-15 and clang-19).
//
// expect: fail
// expect-match: any_unit::assign_to(out) assigns into a unit variable (e.g. meters<double>), not a bare number. Collapse to a unit, then read its value.
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/serialization.h>
using namespace units::literals;
auto make()
{
	auto v = units::deserialize(units::serialize(1.0_m));
	int  target = 0;
	return v->assign_to(target);
}
int main() { (void)make; return 0; }
