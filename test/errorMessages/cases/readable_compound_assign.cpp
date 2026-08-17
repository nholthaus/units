// Case: assigning a compound (derived) result to the wrong named unit must FAIL readably.
// velocity = length / time; assigning it to acceleration_ names both strong types.
//
// expect: fail
// expect-match: meters_per_second<
// expect-match: acceleration
#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>
#include <units/acceleration.h>
using namespace units::literals;
units::acceleration::meters_per_second_squared<double> a = 1.0_m / 1.0_s; // ill-formed: velocity -> acceleration
int main()
{
	(void)a;
	return 0;
}
