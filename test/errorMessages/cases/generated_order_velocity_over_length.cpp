// GENERATED (generate_cases.py). #357-class ordering: an expression reducing to the 'frequency' dimension is
// formed BEFORE that dimension's header is included — must still compile (no explicit-specialization-after-
// instantiation).
// expect: pass
#include <units/velocity.h>
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto x = 1.0_mps / 1.0_m;
#include <units/frequency.h>
int main() { (void)sizeof(x); return 0; }
