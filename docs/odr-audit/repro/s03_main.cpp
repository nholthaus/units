#include <cstdio>
#include <cstddef>
std::size_t from_a();
std::size_t from_b();
int main() { std::printf("a=0x%zx  b=0x%zx\n", from_a(), from_b()); }
