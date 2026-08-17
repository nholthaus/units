// ODR-AUDIT REPRO mul2_trait -- SOURCE: operator* SQUARED (m*m) x SURFACE: user trait specialization
//
// EXPECTED (if the result type were TU-invariant): decltype(m*m) is one fixed type
//   everywhere, so the weak inline classify() selects one AreaPolicy specialization
//   and returns one fixed int.
// OBSERVED: classify()'s R = decltype(m*m) is area::square_meters<double> in a TU where
//   <units/area.h>'s named_class_of registration is visible (the operator* rewrap at
//   core.h ~L3947), else the plain unit<...length^2...> base. The user trait AreaPolicy
//   is specialized ONLY for the named type. So classify() dispatches to the code()==77
//   specialization in one TU and to the primary code()==0 in the other -- two
//   definitions of one weak symbol. The linker keeps ONE; the dispatched int flips
//   77<->0 by LINK ORDER. This is a VALUE/BEHAVIOR divergence, not cosmetic.
//
// EXPECTED vs OBSERVED types (g++/clang, -std=c++23, demangled):
//   WITH  <units/area.h>: units::area::square_meters<double>
//   WITHOUT             : units::unit<units::conversion_factor<std::ratio<1l,1l>,
//                           units::dimension_t<units::dim<units::dimension::length_tag,
//                           std::ratio<2l,1l>>>, ...>, double, units::linear_scale>
//
// The named type is only FORWARD-DECLARED here (enough to name the specialization key)
// so this header does NOT itself pull area's registration; each .cpp decides visibility
// by whether it includes <units/area.h> before this header.
//
// BUILD (both link orders):
//   g++ -std=c++23 -I /e/workspace/units/include -c mul2_trait_a.cpp mul2_trait_b.cpp mul2_trait_main.cpp
//   g++ mul2_trait_a.o mul2_trait_b.o mul2_trait_main.o -o mul2_trait_ab  # order a b -> 77
//   g++ mul2_trait_b.o mul2_trait_a.o mul2_trait_main.o -o mul2_trait_ba  # order b a -> 0
#pragma once
#include <units/length.h>
namespace units { inline namespace area { template<class> struct square_meters; } }
template<class T> struct AreaPolicy { static int code() { return 0; } };
template<> struct AreaPolicy<units::area::square_meters<double>> { static int code() { return 77; } };
inline int classify(double x, double y)
{
	using R = decltype(units::meters<double>(x) * units::meters<double>(y));
	return AreaPolicy<R>::code();
}
