#include <iostream>
#include <string>
std::string from_a();
std::string from_b();
// Both call the SAME weak describe_lm(); with no named type for length/mass, the kept definition is
// identical to the discarded one -> the output is the same in both link orders. Negative control.
int main() { std::cout << "a=" << from_a() << "  b=" << from_b() << "\n"; }
