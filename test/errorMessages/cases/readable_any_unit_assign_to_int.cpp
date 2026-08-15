// any_unit::assign_to(out) rejects a bare int target with a friendly message; it assigns into a UNIT variable.
// expect: fail
// expect-match: assigns into a unit variable
#include <units/length.h>
#include <units/serialization.h>
using namespace units::literals;
auto make()
{
	auto v = units::deserialize(units::serialize(1.0_m));
	int  target = 0;
	return v->assign_to(target);
}
int main() { (void)make; return 0; }
