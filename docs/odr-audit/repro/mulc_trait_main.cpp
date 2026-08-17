#include <cstdio>
int from_a();
int from_b();
// Both call the SAME weak classify(); the linker keeps one definition -> both print it.
int main() { std::printf("a=%d  b=%d\n", from_a(), from_b()); }
