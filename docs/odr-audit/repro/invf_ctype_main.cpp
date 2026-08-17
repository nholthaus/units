#include <cstdio>
int from_a();
int from_b();
// Both call the SAME weak pick_inv(); the linker keeps one definition -> both print its selected code.
// AB (a kept, freq visible) -> "a=1 b=1"; BA (b kept, freq blind) -> "a=2 b=2". The dispatched overload
// flipped purely by link order.
int main() { std::printf("a=%d  b=%d\n", from_a(), from_b()); }
