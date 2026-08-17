// ODR-AUDIT REPRO divs_stable -- SOURCE: operator/ COMPOUND cross-dimension (core.h ~4091)
//                                 SURFACE: type identity / rendered text -- CONTROL (NO divergence)
//
// The task asks: also try a compound whose quotient does NOT rewrap and RECORD it STABLE. This is
// that control. `units::meters<double>{} / units::kilograms<double>{}` has dimension length^1 mass^-1,
// for which NO dimension header registers a named class via UNIT_ADD (there is no "meters_per_kilogram"
// unit). So rewrap_to_named_t hits the void ellipsis fallback and the result STAYS the plain
// units::unit<conversion_factor<ratio<1>, dimension_t<dim<length_tag,1>, dim<mass_tag,-1>>>>, double,
// linear_scale> in EVERY TU -- there is no named form for any header to make visible.
//
// PROOF (typeid): identical demangled type whether the TU includes ONLY length.h + mass.h or ALSO
//   velocity/frequency/force/pressure/energy/power/area/volume -- i.e. no header can perturb it:
//     units::unit<units::conversion_factor<std::ratio<1>, units::dimension_t<
//       units::dim<units::dimension::length_tag, std::ratio<1>>,
//       units::dim<units::dimension::mass_tag,   std::ratio<-1>>>>, double, units::linear_scale>
//
// This repro links a weak inline describe_lm() (renders meters/kilograms via units::to_string) from a
//   TU that includes MANY dimension headers (a.cpp) and one that includes NONE beyond the minimum
//   (b.cpp). Both render identically in BOTH link orders -> no ODR divergence is possible. This is the
//   negative control that isolates the rewrap-ADL mechanism: divergence requires a NAMED type to exist
//   for the result dimension; where none does, the operator is TU-invariant.
//
// BUILD (both link orders):
//   g++ -std=c++23 -I /e/workspace/units/include -c divs_stable_a.cpp divs_stable_b.cpp divs_stable_main.cpp
//   g++ divs_stable_a.o divs_stable_b.o divs_stable_main.o -o divs_stable_ab
//   g++ divs_stable_b.o divs_stable_a.o divs_stable_main.o -o divs_stable_ba
//   ./divs_stable_ab ; ./divs_stable_ba
//
// OBSERVED (g++ 15, clang 20): both orders print identical text for a() and b(). VERDICT: STABLE.
#pragma once
#include <units/length.h>
#include <units/mass.h>
#include <string>

inline std::string describe_lm(double m, double kg)
{
	return units::to_string(units::meters<double>(m) / units::kilograms<double>(kg));
}
