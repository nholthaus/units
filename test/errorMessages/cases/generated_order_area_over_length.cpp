// GENERATED (generate_cases.py). #357-class ordering: an expression reducing to the 'length' dimension is
// formed BEFORE that dimension's header is included — must still compile (no explicit-specialization-after-
// instantiation).
// expect: pass
#include <units/area.h>
#include <units/length.h>
using namespace units;
using namespace units::literals;
auto x = units::area::square_meters<double>(4.0) / 1.0_m;
#include <units/length.h>
int main() { (void)sizeof(x); return 0; }
