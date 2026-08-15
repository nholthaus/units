// visit() requires a generic visitor: a body that names one concrete unit cannot be invoked for every candidate
// dimension, so it fails to compile. (Restrict the candidate set to name a concrete type.)
// expect: fail
// expect-match: call to
#include <units/length.h>
#include <units/time.h>
#include <units/serialization.h>
using namespace units::literals;
void make()
{
	auto v = units::deserialize(units::serialize(1.0_m));
	v->visit([](units::seconds<double>) {}); // ill-formed: not invocable for the length candidate
}
int main() { make(); return 0; }
