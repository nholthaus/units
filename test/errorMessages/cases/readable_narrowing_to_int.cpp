// Case: a lossy conversion into an integer-underlying unit must FAIL readably, naming both types.
// feet -> meters is not an integer-exact ratio, so it cannot bind to meters<int> implicitly.
// The `feet<` token matches the friendly form (`feet<double>` on g++, default-elided `feet<>` on
// clang/MSVC) while rejecting the `feet_` tag and the plain `unit<...>` base. `meters<int>` is kept in
// full because the integer underlying is the point of this case (it proves the result is int-backed):
// `<int>` is a NON-default argument, so no compiler elides it — g++ and clang both print `meters<int>`
// verbatim, making the token portable.
//
// expect: fail
// expect-match: feet<
// expect-match: meters<int>
#include <units/length.h>
using namespace units;
using namespace units::literals;
units::length::meters<int> a = 1.0_ft; // ill-formed: narrowing/lossy into an integer underlying
int main()
{
	(void)a;
	return 0;
}
