// ODR-AUDIT REPRO mul2_text -- SOURCE: operator* SQUARED (m*m) x SURFACE: rendered text
//
// EXPECTED (if the result type were TU-invariant): every TU agrees on the type of
//   meters*meters, so the shared inline describe() renders one fixed string.
// OBSERVED: describe() is a weak inline whose RESULT TYPE is
//   rewrap_to_named_t<...length^2 CF...> (core.h ~L3947, the operator* rewrap). In a
//   TU that included <units/area.h> the ADL map named_class_of resolves the result to
//   area::square_meters (abbreviation "m2"); in a TU that did not, it stays the plain
//   unit<...> base (dimension form "m^2"). Two definitions of one weak symbol -> ODR
//   violation; the linker keeps ONE, chosen by LINK ORDER. VALUE is identical (6.25);
//   only the RENDERED TEXT differs -> COSMETIC harm.
//
// EXPECTED vs OBSERVED types (g++/clang, -std=c++23, demangled):
//   WITH  <units/area.h>: units::area::square_meters<double>
//   WITHOUT             : units::unit<units::conversion_factor<std::ratio<1l,1l>,
//                           units::dimension_t<units::dim<units::dimension::length_tag,
//                           std::ratio<2l,1l>>>, std::ratio<0l,1l>, std::ratio<0l,1l>>,
//                           double, units::linear_scale>
//
// BUILD (both link orders):
//   g++ -std=c++23 -I /e/workspace/units/include -c mul2_text_a.cpp mul2_text_b.cpp mul2_text_main.cpp
//   g++ mul2_text_a.o mul2_text_b.o mul2_text_main.o -o mul2_text_ab   # order a b -> "6.25 m2"
//   g++ mul2_text_b.o mul2_text_a.o mul2_text_main.o -o mul2_text_ba   # order b a -> "6.25 m^2"
#pragma once
#include <units/length.h>
#include <string>
inline std::string describe(double x, double y)
{
	return units::to_string(units::meters<double>(x) * units::meters<double>(y));
}
