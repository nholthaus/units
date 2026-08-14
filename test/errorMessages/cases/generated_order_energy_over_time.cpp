// GENERATED (generate_cases.py). #357-class ordering: an expression reducing to the 'power' dimension is
// formed BEFORE that dimension's header is included — must still compile (no explicit-specialization-after-
// instantiation).
// expect: pass
#include <units/energy.h>
#include <units/time.h>
using namespace units;
using namespace units::literals;
auto x = units::energy::joules<double>(6.0) / 1.0_s;
#include <units/power.h>
int main() { (void)sizeof(x); return 0; }
