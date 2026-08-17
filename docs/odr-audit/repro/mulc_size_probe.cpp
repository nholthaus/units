// ODR-AUDIT REPRO mulc_size -- SOURCE: operator* COMPOUND (kg * m/s^2 == force) x SURFACE: sizeof/alignof/trivial
//
// QUESTION: does the named result type (force::newtons<double>, present WITH
//   <units/force.h>) ever differ in size/alignment/trivial-copyability from the plain
//   unit<...M*L/T^2...> base (present WITHOUT it)?
// OBSERVED: NO. Both forms are sizeof=8, alignof=8, is_trivially_copyable=true. The named
//   class merely DERIVES from the plain unit<...> with only defaulted special members and
//   no added data. The static_asserts below PASS -> the mulc harm is confined to
//   type-IDENTITY (text/trait), never object layout.
//
// BUILD (single TU):
//   g++ -std=c++23 -I /e/workspace/units/include mulc_size_probe.cpp -o mulc_size && ./mulc_size
#include <units/mass.h>
#include <units/acceleration.h>
#include <units/force.h>
#include <type_traits>
#include <cstdio>

using Named     = units::force::newtons<double>;
using Compound  = decltype(units::kilograms<double>(1) * units::meters_per_second_squared<double>(1)); // == newtons here
using PlainBase = units::force::newtons<double>::base; // the unit<...> base directly

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
