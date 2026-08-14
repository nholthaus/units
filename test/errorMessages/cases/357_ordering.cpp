// Case: nholthaus/units #357 — forming an expression that simplifies to a dimension
// (velocity/length -> 1/time == frequency's dimension) BEFORE including that dimension's
// header must NOT cause an "explicit specialization after instantiation" error.
//
// EXPECT: compiles clean (exit 0). A regression here is the #357 bug.
#include <units/velocity.h>
#include <units/length.h>
using namespace units::literals;
auto x = 1.0_mps / 1.0_m; // dimension time^-1, == frequency's dimension, before frequency.h is seen
#include <units/frequency.h>
int main()
{
	(void)x;
	return 0;
}
