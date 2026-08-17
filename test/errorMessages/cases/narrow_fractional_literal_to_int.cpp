// Case: a whole-number literal converts into an integer-backed unit at compile time (feet<int> f = 16_ft),
// but a fractional literal cannot — 16.5 feet is not a whole number of feet, so feet<int> f = 16.5_ft is
// ill-formed. The narrowing constructor is consteval and rejects the non-integer value during constant
// evaluation; the diagnostic names the unit and reports that the value is not a whole number.
//
// The readable signals differ by compiler and are asserted per-compiler to the STRONGEST token each prints:
//   - `feet<int>` (the target unit type) is named by every compiler, so it is asserted universally.
//   - GCC and clang surface the constructor's own plain-language wholeness message (the thrown string
//     "...whole number in range"), so that verbatim reason is the tight -gcc assertion.
//   - MSVC does NOT surface the thrown string; it reports the rejection as C7595 ("call to immediate function
//     is not a constant expression"). That phrase names the REASON MSVC prints and is the tight -msvc assertion.
// Readability is verified two-sided: the target type / reason IS named AND the message is not buried in
// conversion-factor / dimension soup (both forbid tokens confirmed absent on GCC-13/15, clang, and MSVC).
//
// expect: fail
// expect-match: feet<int>
// expect-match-gcc: a floating-point unit converts to an integral unit only when its value is a whole number in range
// expect-match-msvc: call to immediate function is not a constant expression
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/length.h>
using namespace units;
using namespace units::literals;
using namespace units::length;
auto bad = feet<int>{16.5_ft};   // ill-formed: 16.5 is not a whole number of feet
int main()
{
	(void)bad;
	return 0;
}
