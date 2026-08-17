// ODR-AUDIT REPRO mul2_size -- SOURCE: operator* SQUARED (m*m) x SURFACE: sizeof/alignof/trivial-copy
//
// QUESTION: does the named result type (area::square_meters<double>, present WITH
//   <units/area.h>) ever differ in size/alignment/trivial-copyability from the plain
//   unit<...length^2...> base (present WITHOUT it)? If so, an ODR mismatch would be a
//   MEMORY-LAYOUT hazard, not merely cosmetic.
// OBSERVED: NO. Both forms are sizeof=8, alignof=8, is_trivially_copyable=true. The
//   named class merely DERIVES from the plain unit<...> with only defaulted special
//   members and no added data, so layout/triviality are identical. The static_asserts
//   below PASS, proving there is no layout divergence for this source -> the mul2 harm
//   is confined to type-IDENTITY (text/trait), never object layout.
//
// BUILD (single TU; run for the printed values, the static_asserts gate it):
//   g++ -std=c++23 -I /e/workspace/units/include mul2_size_probe.cpp -o mul2_size && ./mul2_size
#include <units/length.h>
#include <units/area.h>
#include <type_traits>
#include <cstdio>

// Named form (rewrap target when area.h is visible):
using Named = units::area::square_meters<double>;
// Plain form (the base the named class derives from; what a TU without area.h sees):
using Plain = decltype(units::meters<double>(1) * units::meters<double>(1)); // == square_meters here
using PlainBase = units::area::square_meters<double>::base;                  // the unit<...> base directly

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
