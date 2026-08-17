// ODR-AUDIT REPRO s04 -- SOURCE: operator/ (m/s) x SURFACE: std::formatter specialization
//
// The user provides std::formatter<> ONLY for the NAMED velocity type (units has no
// built-in formatter for the plain unit<...>). So a weak inline that formats
// decltype(m/s) with std::format:
//   * COMPILES in a TU where decltype(m/s) is the named type (this file), and
//   * FAILS TO COMPILE in a TU where it is the plain unit<...> base (s04_plain.cpp).
// This surface is therefore a per-TU COMPILE SPLIT, not a silent link-order flip: the
// same source line formats or hard-errors depending purely on which headers preceded
// it in the TU. Observable, but caught at compile time rather than corrupting a build.
//
// BUILD:
//   g++ -std=c++23 -I /e/workspace/units/include -c s04_named.cpp   # succeeds
//   g++ -std=c++23 -I /e/workspace/units/include -c s04_plain.cpp   # FAILS (no formatter)
#include <units/velocity.h>   // registration visible -> decltype(m/s) = named -> formatter applies
#include <format>
#include <string>
namespace units { inline namespace velocity { template<class> struct meters_per_second; } }
template<> struct std::formatter<units::velocity::meters_per_second<double>> : std::formatter<std::string>
{
	auto format(const units::velocity::meters_per_second<double>&, auto& ctx) const
	{
		return std::formatter<std::string>::format("SPEED", ctx);
	}
};
std::string fmt_it(double m, double s)
{
	using R = decltype(units::meters<double>(m) / units::seconds<double>(s));
	R v = units::meters<double>(m) / units::seconds<double>(s);
	return std::format("{}", v);
}
