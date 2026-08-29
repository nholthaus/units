// Case: units::torque::foot_pounds is a deprecated alias of the conventionally-named units::torque::pound_feet.
// Using it must compile (a warning, not an error) and the diagnostic must name the recommended replacement.
// (units::energy::foot_pounds is a separate, non-deprecated energy unit and is unaffected.)
//
// LIBRARY-CONTROLLED text: the [[deprecated("...")]] attribute string units wrote (torque.h) is surfaced verbatim by
// the compiler, so the near-verbatim sentence — the recommended replacement AND the energy-unit disambiguation — is
// asserted so a reword regresses the test. The `deprecated` token confirms it is the deprecation diagnostic.
//
// expect: pass
// flags-msvc: /W3
// expect-match: deprecated
// expect-match: torque is conventionally 'pound-foot'; use units::torque::pound_feet. (units::energy::foot_pounds remains the energy unit.)
#include <units/torque.h>
units::torque::foot_pounds<double> t(1.0); // compiles; the diagnostic under test is a WARNING, not an error
int main()
{
	(void)t;
	return 0;
}
