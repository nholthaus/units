// unit_cast<T>(any_unit) rejects a bare arithmetic target with a friendly message.
// expect: fail
// expect-match: casts to a unit type
#include <units/length.h>
#include <units/serialization.h>
using namespace units::literals;
auto make()
{
	auto v = units::deserialize(units::serialize(1.0_m));
	return units::unit_cast<long>(*v);
}
int main() { (void)make; return 0; }
