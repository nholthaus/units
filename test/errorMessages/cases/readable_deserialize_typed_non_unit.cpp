// deserialize<T>(bytes) rejects a non-unit T with a friendly message directing to the erased form.
// expect: fail
// expect-match: decodes into a unit type
#include <units/length.h>
#include <units/serialization.h>
using namespace units::literals;
auto make() { return units::deserialize<double>(units::serialize(1.0_m)); }
int main() { (void)make; return 0; }
