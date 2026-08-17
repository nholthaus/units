#include <cstdio>
unsigned long call_layout_a();
unsigned long call_layout_b();
// layout_probe is a FIXED-signature weak inline -> both TUs emit the SAME mangled symbol, the linker
// keeps ONE. We print sizeof(Holder) (encoded as size*1000+value) from both paths and both link orders
// to show the layout is identical (benign-same-layout): "size=8 val=0" everywhere. The signature-split
// (consume_named) is shown separately by the nm dump in run.sh.
int main()
{
	unsigned long a = call_layout_a();
	unsigned long b = call_layout_b();
	std::printf("a: size=%lu val=%lu   b: size=%lu val=%lu   layout match: %s\n",
	            a / 1000, a % 1000, b / 1000, b % 1000, (a == b) ? "YES" : "NO");
}
