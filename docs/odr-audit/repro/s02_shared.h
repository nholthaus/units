// ODR-AUDIT REPRO s02 -- SOURCE: operator/ (compound m/s) x SURFACE: type-dispatched value
//
// EXPECTED (if the result type were TU-invariant): decltype(m/s) is one fixed type
//   everywhere, so the weak inline classify() selects one SpeedPolicy specialization
//   and returns one fixed int.
// OBSERVED: classify()'s R = decltype(m/s) is velocity::meters_per_second<double> in
//   a TU where <units/velocity.h>'s named_class_of registration is visible, else the
//   plain unit<...> base. The user trait SpeedPolicy is specialized ONLY for the named
//   type. So classify() dispatches to the code()==42 specialization in one TU and to
//   the primary code()==0 in the other -- two definitions of one weak symbol. The
//   linker keeps ONE; the dispatched int flips 42<->0 by LINK ORDER. This is a
//   VALUE/BEHAVIOR divergence, not cosmetic.
//
// The named type is only FORWARD-DECLARED here (enough to name the specialization
// key) so this header does NOT itself pull velocity's registration; each .cpp
// decides visibility by whether it includes <units/velocity.h> before this header.
#pragma once
#include <units/length.h>
#include <units/time.h>
namespace units { inline namespace velocity { template<class> struct meters_per_second; } }
template<class T> struct SpeedPolicy { static int code() { return 0; } };
template<> struct SpeedPolicy<units::velocity::meters_per_second<double>> { static int code() { return 42; } };
inline int classify(double m, double s)
{
	using R = decltype(units::meters<double>(m) / units::seconds<double>(s));
	return SpeedPolicy<R>::code();
}
