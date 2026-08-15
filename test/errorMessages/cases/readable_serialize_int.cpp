// serialize() rejects an int (not a unit) with a friendly message, no template soup.
// expect: fail
// expect-match: serialize requires a units quantity
#include <units/serialization.h>
auto bad = units::serialize(42);
int main() { (void)bad; return 0; }
