// ODR-AUDIT REPRO mulc_text -- SOURCE: operator* COMPOUND (kg * m/s^2 == force) x SURFACE: rendered text
//
// EXPECTED (if the result type were TU-invariant): every TU agrees on the type of
//   kilograms * meters_per_second_squared, so the shared inline describe() renders one
//   fixed string.
// OBSERVED: describe() is a weak inline whose RESULT TYPE is
//   rewrap_to_named_t<...M*L/T^2 CF...> (core.h ~L3960, the cross-dimension operator*
//   rewrap). VERIFIED via typeid probe that this compound CF DOES land on the named
//   force type: in a TU that included <units/force.h> the ADL map named_class_of
//   resolves the result to force::newtons (abbreviation "N"); in a TU that did not, it
//   stays the plain unit<...> base (dimension form "m kg s^-2"). Two definitions of one
//   weak symbol -> ODR violation; the linker keeps ONE, chosen by LINK ORDER. VALUE is
//   identical (20); only the RENDERED TEXT differs -> COSMETIC harm.
//
// EXPECTED vs OBSERVED types (g++/clang, -std=c++23, demangled):
//   WITH  <units/force.h>: units::force::newtons<double>
//   WITHOUT              : units::unit<units::conversion_factor<std::ratio<1l,1l>,
//                            units::dimension_t<units::dim<units::dimension::length_tag,std::ratio<1l,1l>>,
//                            units::dim<units::dimension::mass_tag,std::ratio<1l,1l>>,
//                            units::dim<units::dimension::time_tag,std::ratio<-2l,1l>>>, ...>,
//                            double, units::linear_scale>
//
// BUILD (both link orders):
//   g++ -std=c++23 -I /e/workspace/units/include -c mulc_text_a.cpp mulc_text_b.cpp mulc_text_main.cpp
//   g++ mulc_text_a.o mulc_text_b.o mulc_text_main.o -o mulc_text_ab  # order a b -> "20 N"
//   g++ mulc_text_b.o mulc_text_a.o mulc_text_main.o -o mulc_text_ba  # order b a -> "20 m kg s^-2"
#pragma once
#include <units/mass.h>
#include <units/acceleration.h>
#include <string>
inline std::string describe(double m, double a)
{
	return units::to_string(units::kilograms<double>(m) * units::meters_per_second_squared<double>(a));
}
