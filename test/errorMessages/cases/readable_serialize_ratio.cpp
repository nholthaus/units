// serialize() rejects a std::ratio (not a unit) with a friendly message.
// expect: fail
// expect-match: serialize requires a units quantity
#include <ratio>
#include <units/serialization.h>
auto bad = units::serialize(std::ratio<1, 2>{});
int main() { (void)bad; return 0; }
