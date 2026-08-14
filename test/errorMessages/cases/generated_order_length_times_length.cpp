// GENERATED (generate_cases.py). #357-class ordering: an expression reducing to the 'area' dimension is
// formed BEFORE that dimension's header is included — must still compile (no explicit-specialization-after-
// instantiation).
// expect: pass
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto x = 1.0_m * 1.0_m;
#include <units/area.h>
int main() { (void)sizeof(x); return 0; }
