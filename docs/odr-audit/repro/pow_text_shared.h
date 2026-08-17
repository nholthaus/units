// ODR-AUDIT REPRO pow_text -- SOURCE: units::pow<2>(m) x SURFACE: rendered text
//
// EXPECTED (if the result type were TU-invariant): every TU agrees on the type of
//   pow<2>(meters), so the shared inline describe() renders one fixed string.
// OBSERVED: describe() is a weak inline whose RESULT TYPE is
//   rewrap_to_named_t<unit<power_of_unit<2, length CF>...>> (core.h ~L4459, the pow<N>
//   rewrap). In a TU that included <units/area.h> the ADL map named_class_of resolves the
//   result to area::square_meters (abbreviation "m2"); in a TU that did not, it stays the
//   plain unit<...length^2...> base (dimension form "m^2"). Identical result type and
//   demangling to the m*m squared-multiply source -- pow<2> and operator* share the same
//   rewrap target. Two definitions of one weak symbol -> ODR violation; the linker keeps
//   ONE, chosen by LINK ORDER. VALUE identical (6.25); only RENDERED TEXT differs ->
//   COSMETIC harm.
//
// EXPECTED vs OBSERVED types (g++/clang, -std=c++23, demangled):
//   WITH  <units/area.h>: units::area::square_meters<double>
//   WITHOUT             : units::unit<units::conversion_factor<std::ratio<1l,1l>,
//                           units::dimension_t<units::dim<units::dimension::length_tag,
//                           std::ratio<2l,1l>>>, std::ratio<0l,1l>, std::ratio<0l,1l>>,
//                           double, units::linear_scale>
//
// BUILD (both link orders):
//   g++ -std=c++23 -I /e/workspace/units/include -c pow_text_a.cpp pow_text_b.cpp pow_text_main.cpp
//   g++ pow_text_a.o pow_text_b.o pow_text_main.o -o pow_text_ab  # order a b -> "6.25 m2"
//   g++ pow_text_b.o pow_text_a.o pow_text_main.o -o pow_text_ba  # order b a -> "6.25 m^2"
#pragma once
#include <units/length.h>
#include <string>
inline std::string describe(double x)
{
	return units::to_string(units::pow<2>(units::meters<double>(x)));
}
