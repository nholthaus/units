#include <iostream>
#include <string>
std::string from_a();
std::string from_b();
// Both call the SAME weak describe(); whichever definition the linker kept prints
// for BOTH -> the divergence is visible as the link-order-dependent output.
int main() { std::cout << "a=" << from_a() << "  b=" << from_b() << "\n"; }
