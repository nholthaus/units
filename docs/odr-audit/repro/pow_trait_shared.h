// ODR-AUDIT REPRO pow_trait -- SOURCE: units::pow<2>(m) x SURFACE: user trait specialization
//
// EXPECTED (if the result type were TU-invariant): decltype(pow<2>(m)) is one fixed type
//   everywhere, so the weak inline classify() selects one AreaPolicy specialization and
//   returns one fixed int.
// OBSERVED: classify()'s R = decltype(pow<2>(m)) is area::square_meters<double> in a TU
//   where <units/area.h>'s named_class_of registration is visible (the pow<N> rewrap at
//   core.h ~L4459), else the plain unit<...length^2...> base. The user trait AreaPolicy is
//   specialized ONLY for the named type. So classify() dispatches to code()==55 in one TU
//   and to the primary code()==0 in the other -- two definitions of one weak symbol. The
//   linker keeps ONE; the dispatched int flips 55<->0 by LINK ORDER. VALUE/BEHAVIOR
//   divergence, not cosmetic.
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
//   g++ -std=c++23 -I /e/workspace/units/include -c pow_trait_a.cpp pow_trait_b.cpp pow_trait_main.cpp
//   g++ pow_trait_a.o pow_trait_b.o pow_trait_main.o -o pow_trait_ab  # order a b -> 55
//   g++ pow_trait_b.o pow_trait_a.o pow_trait_main.o -o pow_trait_ba  # order b a -> 0
#pragma once
#include <units/length.h>
namespace units { inline namespace area { template<class> struct square_meters; } }
template<class T> struct AreaPolicy { static int code() { return 0; } };
template<> struct AreaPolicy<units::area::square_meters<double>> { static int code() { return 55; } };
inline int classify(double x)
{
	using R = decltype(units::pow<2>(units::meters<double>(x)));
	return AreaPolicy<R>::code();
}
