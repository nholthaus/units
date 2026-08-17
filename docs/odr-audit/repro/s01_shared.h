// ODR-AUDIT REPRO s01 -- SOURCE: operator/ (compound m/s) x SURFACE: rendered text
//
// EXPECTED (if the result type were TU-invariant): every TU agrees on the type of
//   meters/seconds, so the shared inline describe() renders one fixed string.
// OBSERVED: describe() is a weak inline whose RESULT TYPE is
//   rewrap_to_named_t<...velocity CF...>. In a TU that included <units/velocity.h>
//   the ADL map named_class_of resolves the result to velocity::meters_per_second
//   (abbreviation "mps"); in a TU that did not, it stays the plain unit<...> base
//   (dimension form "m s^-1"). Two definitions of one weak symbol -> ODR violation;
//   the linker keeps ONE, chosen by LINK ORDER. VALUE is identical (2.5); only the
//   RENDERED TEXT differs -> COSMETIC harm.
//
// BUILD (see run.sh for the automated both-orders check):
//   g++ -std=c++23 -I /e/workspace/units/include -c s01_a.cpp s01_b.cpp s01_main.cpp
//   g++ s01_a.o s01_b.o s01_main.o -o s01_ab   # link order a b -> "2.5 mps"
//   g++ s01_b.o s01_a.o s01_main.o -o s01_ba   # link order b a -> "2.5 m s^-1"
#pragma once
#include <units/length.h>
#include <units/time.h>
#include <string>
inline std::string describe(double m, double s)
{
	return units::to_string(units::meters<double>(m) / units::seconds<double>(s));
}
