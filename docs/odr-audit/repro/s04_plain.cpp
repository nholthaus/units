// See s04_named.cpp. This TU does NOT include <units/velocity.h>, so decltype(m/s) is
// the plain unit<...> base, for which NO std::formatter exists -> HARD COMPILE ERROR.
// Compiling this file is EXPECTED TO FAIL; that failure IS the divergence.
#include <units/length.h>
#include <units/time.h>
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
	using R = decltype(units::meters<double>(m) / units::seconds<double>(s));   // plain unit<...>
	R v = units::meters<double>(m) / units::seconds<double>(s);
	return std::format("{}", v);   // no formatter for plain unit<...> -> compile error
}
