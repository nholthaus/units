// GENERATED (generate_cases.py). #357-class ordering: an expression reducing to the 'force' dimension is
// formed BEFORE that dimension's header is included — must still compile (no explicit-specialization-after-
// instantiation).
// expect: pass
#include <units/mass.h>
#include <units/acceleration.h>
using namespace units;
using namespace units::literals;
auto x = 1.0_kg * units::acceleration::meters_per_second_squared<double>(1.0);
#include <units/force.h>
int main() { (void)sizeof(x); return 0; }
