// Case: a whole-number-of-bytes bit count converts into a coarser integer-backed unit at compile time
// (bytes<int> b = bits<int>(16) is 2 bytes), but an indivisible one cannot — 17 bits is not a whole number
// of bytes, so bytes<int>{bits<int>(17)} is ill-formed. The exact-integral narrowing constructor is consteval
// and rejects the indivisible value during constant evaluation; the diagnostic names the target unit and
// reports that the value is not a whole number of it. This is the integer counterpart of the fractional
// float->int narrowing case (16.5_ft -> feet<int>).
//
// The readable signals differ by compiler and are asserted per-compiler to the STRONGEST token each prints:
//   - `bytes<int>` (the target unit type) is named by every compiler, so it is asserted universally.
//   - GCC and clang surface the constructor's own plain-language message (the thrown string
//     "...exact whole number of the target unit"), so that verbatim reason is the tight -gcc assertion.
//   - MSVC does NOT surface the thrown string; it reports the rejection as C7595 ("call to immediate function
//     is not a constant expression"). That phrase names the REASON MSVC prints and is the tight -msvc assertion.
// Readability is verified two-sided: the target type / reason IS named AND the message is not buried in
// conversion-factor / dimension soup (both forbid tokens confirmed absent on GCC-13/15, clang, and MSVC).
//
// expect: fail
// expect-match: bytes<int>
// expect-match-gcc: an integral unit converts to a coarser integral unit only when the value is an exact whole number of the target unit
// expect-match-msvc: call to immediate function is not a constant expression
// forbid-match: conversion_factor<std::ratio<1>, units::dimension_t
// forbid-match: dimension_t<
#include <units/data.h>
using namespace units;
using namespace units::data;
auto bad = bytes<int>{bits<int>(17)};   // ill-formed: 17 bits is not a whole number of bytes
int main()
{
	(void)bad;
	return 0;
}
