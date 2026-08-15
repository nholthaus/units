// any_unit::to<T>() rejects a bare int target with a friendly message; to<> collapses to a UNIT.
// expect: fail
// expect-match: collapses into a unit type
#include <units/length.h>
#include <units/serialization.h>
using namespace units::literals;
auto make()
{
	auto v = units::deserialize(units::serialize(1.0_m));
	return v->to<int>();
}
int main() { (void)make; return 0; }
