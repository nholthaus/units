// ODR-AUDIT REPRO s05 -- SOURCE: operator/ (compound m/s) x SURFACE: overload resolution
//
// EXPECTED: dispatch(m/s) always selects the same tag() overload -> one fixed code.
// OBSERVED: two tag() overloads are visible -- one on the NAMED velocity type, one on
//   the plain unit<...> base. Weak inline dispatch() calls tag(m/s). Where decltype(m/s)
//   is the named type, the named overload is the exact match (code 1); where it is the
//   plain unit<...>, only the plain overload matches (code 2). One weak symbol, two
//   bodies -> linker keeps ONE -> the selected overload + returned code flip by LINK
//   ORDER. VALUE/BEHAVIOR harm (silent wrong dispatch across a TU boundary).
//
// BUILD: see run.sh. Link order a b -> "1 1"; link order b a -> "2 2".
#pragma once
#include <units/length.h>
#include <units/time.h>
namespace units { inline namespace velocity { template<class> struct meters_per_second; } }
inline int tag(const units::velocity::meters_per_second<double>&) { return 1; }
inline int tag(const units::unit<units::conversion_factor<std::ratio<1>, units::dimension_t<units::dim<units::dimension::length_tag, std::ratio<1>>, units::dim<units::dimension::time_tag, std::ratio<-1>>>>, double, units::linear_scale>&) { return 2; }
inline int dispatch(double m, double s)
{
	return tag(units::meters<double>(m) / units::seconds<double>(s));
}
