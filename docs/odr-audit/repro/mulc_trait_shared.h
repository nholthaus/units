// ODR-AUDIT REPRO mulc_trait -- SOURCE: operator* COMPOUND (kg * m/s^2 == force) x SURFACE: user trait
//
// EXPECTED (if the result type were TU-invariant): decltype(kg * m/s^2) is one fixed
//   type everywhere, so the weak inline classify() selects one ForcePolicy specialization
//   and returns one fixed int.
// OBSERVED: classify()'s R = decltype(kg * m/s^2) is force::newtons<double> in a TU where
//   <units/force.h>'s named_class_of registration is visible (the cross-dimension
//   operator* rewrap at core.h ~L3960), else the plain unit<...M*L/T^2...> base. The user
//   trait ForcePolicy is specialized ONLY for the named type. So classify() dispatches to
//   the code()==98 specialization in one TU and to the primary code()==0 in the other --
//   two definitions of one weak symbol. The linker keeps ONE; the dispatched int flips
//   98<->0 by LINK ORDER. This is a VALUE/BEHAVIOR divergence, not cosmetic.
//
// EXPECTED vs OBSERVED types (g++/clang, -std=c++23, demangled):
//   WITH  <units/force.h>: units::force::newtons<double>
//   WITHOUT              : units::unit<units::conversion_factor<std::ratio<1l,1l>,
//                            units::dimension_t<units::dim<units::dimension::length_tag,std::ratio<1l,1l>>,
//                            units::dim<units::dimension::mass_tag,std::ratio<1l,1l>>,
//                            units::dim<units::dimension::time_tag,std::ratio<-2l,1l>>>, ...>,
//                            double, units::linear_scale>
//
// The named type is only FORWARD-DECLARED here (enough to name the specialization key)
// so this header does NOT itself pull force's registration; each .cpp decides visibility
// by whether it includes <units/force.h> before this header.
//
// BUILD (both link orders):
//   g++ -std=c++23 -I /e/workspace/units/include -c mulc_trait_a.cpp mulc_trait_b.cpp mulc_trait_main.cpp
//   g++ mulc_trait_a.o mulc_trait_b.o mulc_trait_main.o -o mulc_trait_ab  # order a b -> 98
//   g++ mulc_trait_b.o mulc_trait_a.o mulc_trait_main.o -o mulc_trait_ba  # order b a -> 0
#pragma once
#include <units/mass.h>
#include <units/acceleration.h>
namespace units { inline namespace force { template<class> struct newtons; } }
template<class T> struct ForcePolicy { static int code() { return 0; } };
template<> struct ForcePolicy<units::force::newtons<double>> { static int code() { return 98; } };
inline int classify(double m, double a)
{
	using R = decltype(units::kilograms<double>(m) * units::meters_per_second_squared<double>(a));
	return ForcePolicy<R>::code();
}
