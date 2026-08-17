// ODR-AUDIT REPRO s03 -- SOURCE: operator/ (compound m/s) x SURFACE: std::hash specialization
//
// EXPECTED: one fixed hash for a given m/s value everywhere.
// OBSERVED: a user std::hash<> specialization keyed on the named velocity type applies
//   in a TU that saw the registration and does NOT in one that did not. The weak inline
//   hash_of() computes std::hash<decltype(m/s)>{}(v); it selects the custom specialization
//   (returns 0xBEEF) in one TU and the generic std::hash (some library value) in the other.
//   Linker keeps ONE -> the hash flips by LINK ORDER. VALUE/BEHAVIOR harm: a hashed
//   container keyed on the result type can silently disagree across TUs.
#pragma once
#include <units/length.h>
#include <units/time.h>
#include <cstddef>
#include <functional>
namespace units { inline namespace velocity { template<class> struct meters_per_second; } }
template<> struct std::hash<units::velocity::meters_per_second<double>> {
	std::size_t operator()(const units::velocity::meters_per_second<double>&) const noexcept { return 0xBEEF; }
};
inline std::size_t hash_of(double m, double s)
{
	using R = decltype(units::meters<double>(m) / units::seconds<double>(s));
	R v = units::meters<double>(m) / units::seconds<double>(s);
	return std::hash<R>{}(v);
}
