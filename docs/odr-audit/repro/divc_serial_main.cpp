#include "divc_serial_shared.h"
#include <cstdio>
SerRecord from_a();
SerRecord from_b();
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
