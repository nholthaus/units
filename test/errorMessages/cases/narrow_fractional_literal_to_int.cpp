// Case: a whole-number literal converts into an integer-backed unit at compile time (feet<int> f = 16_ft),
// but a fractional literal cannot — 16.5 feet is not a whole number of feet, so feet<int> f = 16.5_ft is
// ill-formed. The narrowing constructor is consteval and rejects the non-integer value during constant
// evaluation; the diagnostic names the unit and reports that the value is not a whole number.
//
// The diagnostic names the target unit and carries the constructor's own wholeness message, which is a
// string literal and therefore identical across compilers (g++/clang/MSVC) — a stable token to match on.
//
// expect: fail
// expect-match: feet
// expect-match: whole number
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
