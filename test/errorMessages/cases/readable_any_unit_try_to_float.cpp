// any_unit::try_to<T>() rejects a bare float target with a friendly message.
// expect: fail
// expect-match: collapses into a unit type
#include <units/length.h>
#include <units/serialization.h>
using namespace units::literals;
auto make()
{
	auto v = units::deserialize(units::serialize(1.0_m));
	return v->try_to<float>();
}
int main() { (void)make; return 0; }
