// Case: a well-formed value-spec + unit-opts spec compiles clean (the positive control for the format grammar).
// expect: pass
#include <format>
#include <string>
#include <units.h>
using namespace units::literals;
int main()
{
	std::string a = std::format("{:.2f%n}", 6.0_ft);   // precision + full-name label
	std::string b = std::format("{:>10.1f%a'_'}", 3.5_m); // width/align + abbreviation + separator
	std::string c = std::format("{:%v}", 9.81_mps);    // value only
	std::string d = std::format("{:%b}", 9.81_mps);    // base-SI conversion
	(void)a;
	(void)b;
	(void)c;
	(void)d;
	return 0;
}
