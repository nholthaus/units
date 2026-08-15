// any_unit::to<T>() rejects a std::ratio target with a friendly message.
// expect: fail
// expect-match: collapses into a unit type
#include <ratio>
#include <units/length.h>
#include <units/serialization.h>
using namespace units::literals;
auto make()
{
	auto v = units::deserialize(units::serialize(1.0_m));
	return v->to<std::ratio<1>>();
}
int main() { (void)make; return 0; }
