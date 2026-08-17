// Case: adding incompatible units (length + time) must FAIL readably, naming the strong types.
//
// COMPILER-CONTROLLED text: the diagnostic is the compiler's own no-matching-`operator+` / invalid-operands wording
// (g++ writes "no match for 'operator+'", clang "invalid operands to binary expression", MSVC "binary '+': ... does
// not define this operator" and names the candidate as `operator +` with a space), so its exact sentence is not
// asserted verbatim across compilers. Instead the tight readable tokens are asserted: the two FRIENDLY strong types
// AND that the failing operator context is named — g++/clang spell it tight (`operator+`), MSVC inserts a space
// (`operator +`), so the operator token is per-compiler. Anti-soup guards confirm the message does not descend into
// conversion_factor / dimension_t template internals (both markers confirmed absent on GCC-15, clang-19, and MSVC).
//
// expect: fail
// expect-match: meters<
// expect-match: seconds<
// expect-match-gcc: operator+
// expect-match-msvc: operator +
// forbid-match: conversion_factor<std::ratio
// forbid-match: dimension_t<
#include <units/length.h>
#include <units/time.h>
using namespace units::literals;
auto bad = 1.0_m + 1.0_s; // ill-formed: cannot add length and time
int main()
{
	(void)bad;
	return 0;
}
