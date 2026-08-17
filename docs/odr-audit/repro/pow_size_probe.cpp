// ODR-AUDIT REPRO pow_size -- SOURCE: units::pow<2>(m) x SURFACE: sizeof/alignof/trivial-copy
//
// QUESTION: does the named result type (area::square_meters<double>, present WITH
//   <units/area.h>) ever differ in size/alignment/trivial-copyability from the plain
//   unit<...length^2...> base (present WITHOUT it)?
// OBSERVED: NO. Both forms are sizeof=8, alignof=8, is_trivially_copyable=true. Same
//   result type as the m*m squared-multiply source; the named class only derives from the
//   plain unit<...> with defaulted special members and no data. static_asserts PASS -> the
//   pow<2> harm is confined to type-IDENTITY (text/trait), never object layout.
//
// BUILD (single TU):
//   g++ -std=c++23 -I /e/workspace/units/include pow_size_probe.cpp -o pow_size && ./pow_size
#include <units/length.h>
#include <units/area.h>
#include <type_traits>
#include <cstdio>

using Named     = units::area::square_meters<double>;
using Powered   = decltype(units::pow<2>(units::meters<double>(1))); // == square_meters here
using PlainBase = units::area::square_meters<double>::base;          // the unit<...> base directly

static_assert(sizeof(Named) == sizeof(PlainBase), "size differs");
static_assert(alignof(Named) == alignof(PlainBase), "align differs");
static_assert(std::is_trivially_copyable_v<Named> == std::is_trivially_copyable_v<PlainBase>, "trivial-copy differs");

int main()
{
	std::printf("Named    : sizeof=%zu alignof=%zu trivialcopy=%d\n",
		sizeof(Named), alignof(Named), (int)std::is_trivially_copyable_v<Named>);
	std::printf("PlainBase: sizeof=%zu alignof=%zu trivialcopy=%d\n",
		sizeof(PlainBase), alignof(PlainBase), (int)std::is_trivially_copyable_v<PlainBase>);
	std::printf("LAYOUT DIVERGENCE: %s\n",
		(sizeof(Named) != sizeof(PlainBase) || alignof(Named) != alignof(PlainBase) ||
		 std::is_trivially_copyable_v<Named> != std::is_trivially_copyable_v<PlainBase>)
			? "YES" : "NO");
}
