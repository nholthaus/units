// Case: integer presentation types on an integer-underlying named unit compile clean — the value formatter
// delegate is the underlying integer type, so the standard int grammar (x/#06x/b/d) passes through.
// expect: pass
#include <format>
#include <string>
#include <units.h>
int main()
{
	const units::meters<int> mi(255);
	std::string a = std::format("{:x}", mi);
	std::string b = std::format("{:#06x}", mi);
	std::string c = std::format("{:b}", mi);
	std::string d = std::format("{:d%v}", mi);
	(void)a;
	(void)b;
	(void)c;
	(void)d;
	return 0;
}
