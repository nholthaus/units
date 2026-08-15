// Case: units::torque::foot_pounds is a deprecated alias of the conventionally-named units::torque::pound_feet.
// Using it must compile (a warning, not an error) and the diagnostic must name the recommended replacement.
// (units::energy::foot_pounds is a separate, non-deprecated energy unit and is unaffected.)
//
// expect: pass
// expect-match: deprecated
// expect-match: pound_feet
#include <units/torque.h>
units::torque::foot_pounds<double> t(1.0); // deprecated alias of units::torque::pound_feet
int main()
{
	(void)t;
	return 0;
}
