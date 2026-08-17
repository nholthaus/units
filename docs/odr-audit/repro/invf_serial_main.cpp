#include "invf_serial_shared.h"
#include <cstdio>
SerRecord from_a();
SerRecord from_b();
// Both call the SAME weak serialize_inv(); the linker keeps ONE definition -> both report it.
// If serialization leaked the named-type divergence, a() and b() -- or the two link orders --
// would differ. They do not: the wire form is dimension-keyed, so this prints identically both ways.
int main()
{
	SerRecord a = from_a();
	SerRecord b = from_b();
	std::printf("a: bytes = %s   to_string = \"%s\"\n", to_hex(a.bytes).c_str(), a.text.c_str());
	std::printf("b: bytes = %s   to_string = \"%s\"\n", to_hex(b.bytes).c_str(), b.text.c_str());
	std::printf("bytes match: %s   text match: %s\n",
	            (a.bytes == b.bytes) ? "YES" : "NO",
	            (a.text == b.text) ? "YES" : "NO");
}
