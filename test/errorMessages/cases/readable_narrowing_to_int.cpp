// Case: a lossy conversion into an integer-underlying unit must FAIL readably. feet -> meters is not an
// integer-exact ratio, so 1.0_ft cannot bind to meters<int>. The binding runs through a consteval narrowing
// constructor whose compile-time check throws, so the compiler rejects it as a non-constant expression.
//
// The readable signals differ by compiler and are asserted per-compiler to the STRONGEST token each prints:
//   - `meters<int>` (the destination unit type) is named by every compiler, so it is asserted universally.
//   - GCC and clang surface the library's plain-language REASON — the thrown string
//     ("...whole number in range") — so that verbatim reason is the tight -gcc assertion (stronger and more
//     portable than `feet<double>`, which GCC prints but clang does NOT: clang spells the source operand as the
//     `feet_` conversion-factor tag, so `feet<double>` cannot be asserted across GCC and clang together).
//   - MSVC surfaces neither the thrown string nor `feet<double>`; it reports the rejection as C7595
//     ("call to immediate function is not a constant expression"). That phrase names the REASON MSVC prints, so
//     it is the tight -msvc assertion, alongside the universal `meters<int>`.
// Readability is verified two-sided: the destination type / reason IS named AND the message is not buried in
// conversion-factor / dimension soup (both forbid tokens confirmed absent on GCC-13/15, clang, and MSVC).
//
// expect: fail
// expect-match: meters<int>
// expect-match-gcc: a floating-point unit converts to an integral unit only when its value is a whole number in range
// expect-match-msvc: call to immediate function is not a constant expression
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/length.h>
using namespace units;
using namespace units::literals;
units::length::meters<int> a = 1.0_ft; // ill-formed: narrowing/lossy into an integer underlying
int main()
{
	(void)a;
	return 0;
}
