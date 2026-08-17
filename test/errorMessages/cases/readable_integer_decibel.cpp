// Case: a decibel-scale unit with an integral underlying type must FAIL readably.
// A decibel stores its value through a base-10 logarithm, so an integer cannot represent it (3 dB would
// store as 0, large values overflow). The static_assert names the reason.
//
// LIBRARY-CONTROLLED text: the near-verbatim static_assert sentence units emits (core.h) is asserted — including the
// parenthetical reason — so a reword regresses the test. No anti-soup forbid is asserted here: a `decibels<int>`
// IS the dimensionless conversion factor, so its constructor-instantiation backtrace legitimately spells
// `conversion_factor<std::ratio<1>, units::dimension_t<>>` — that is the unavoidable instantiation context of a
// dimensionless decibel, not a readability regression. The static_assert sentence is what the user reads.
//
// expect: fail
// expect-match: a decibel-scale unit requires a floating-point underlying type (an integral type cannot represent a logarithmic value)
#include <units/core.h>
using namespace units;
units::decibels<int> a(3); // ill-formed: integral underlying type on a decibel scale
int main()
{
	(void)a;
	return 0;
}
