// serialize() rejects a std::string (not a unit) with a friendly message.
// expect: fail
// expect-match: serialize requires a units quantity
#include <string>
#include <units/serialization.h>
auto bad = units::serialize(std::string("hi"));
int main() { (void)bad; return 0; }
