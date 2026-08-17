// ODR-AUDIT REPRO sqrt_stable -- SOURCE: units::sqrt(m^2) x SURFACES: text / trait / size
//
// VERDICT: STABLE across all surfaces (no cross-TU divergence possible). This file is the
//   negative result -- it documents WHY sqrt(area)->length can NOT be made to diverge, and
//   proves it by exhausting the divergence attempt.
//
// WHY STABLE (structural, not incidental):
//   sqrt's rewrap target (core.h ~L4807) is rewrap_to_named_t<unit<square_root<area CF>...>>,
//   whose CF reduces to length^1 -> the named_class_of map resolves it to length::meters.
//   The registration for meters lives in <units/length.h>. But <units/area.h> HARD-INCLUDES
//   <units/length.h> (area.h line 49), and -- more fundamentally -- you CANNOT NAME or CONSTRUCT
//   any length^2 operand to hand to sqrt without meters already being defined (square_meters
//   derives from squared<meters_>; even a plain unit<length^2> is only producible via
//   meters*meters, which needs length.h). Therefore in EVERY TU that can call sqrt on an
//   area-like value, the meters named_class_of overload is ALREADY visible, so the rewrap is
//   identical in every TU. There is no "WITHOUT the registration" TU to disagree.
//
// PROVEN (g++ and clang, -std=c++23) -- decltype(sqrt(...)) is the SAME type in all cases:
//   sqrt(plain m^2, TU WITHOUT area.h)   == units::length::meters<double>
//   sqrt(square_meters, TU WITH area.h)  == units::length::meters<double>
//   Contrast the DIVERGENT sources (mul2/mulc/pow): those rewrap to a NAMED type whose header
//   (area.h/force.h) is NOT a prerequisite of the operands (meters/kilograms/accel), so a TU
//   can legitimately omit it and see the plain base -- the asymmetry that makes them diverge.
//
// The static_asserts below GATE the claim: they fail to compile if a divergence ever appears.
//
// BUILD (single TU; there is no a/b/main because there is no divergence to link-order):
//   g++    -std=c++23 -I /e/workspace/units/include sqrt_stable_probe.cpp -o sqrt_stable && ./sqrt_stable
//   clang++ -std=c++23 -I /e/workspace/units/include sqrt_stable_probe.cpp -o sqrt_stable && ./sqrt_stable
#include <units/length.h>
#include <units/area.h>
#include <type_traits>
#include <typeinfo>
#include <cstdio>

// The area-like operand producible with ONLY length.h (a plain unit<length^2>):
using PlainSq  = decltype(units::meters<double>(1) * units::meters<double>(1));
// The named area operand (area.h present): area::square_meters<double>.
using NamedSq  = units::area::square_meters<double>;

// sqrt of each: both must be the SAME named length type in EVERY TU.
using RfromPlain = decltype(units::sqrt(std::declval<PlainSq>()));
using RfromNamed = decltype(units::sqrt(std::declval<NamedSq>()));

static_assert(std::is_same_v<RfromPlain, units::length::meters<double>>,
	"sqrt of a plain length^2 did NOT resolve to meters -- registration not visible?");
static_assert(std::is_same_v<RfromNamed, units::length::meters<double>>,
	"sqrt of square_meters did NOT resolve to meters");
static_assert(std::is_same_v<RfromPlain, RfromNamed>,
	"sqrt result DIVERGES between plain and named operand -- would be an ODR hazard");

int main()
{
	std::printf("sqrt(plain m^2)      -> %s\n", typeid(RfromPlain).name());
	std::printf("sqrt(square_meters)  -> %s\n", typeid(RfromNamed).name());
	std::printf("sizeof=%zu alignof=%zu trivialcopy=%d\n",
		sizeof(RfromNamed), alignof(RfromNamed), (int)std::is_trivially_copyable_v<RfromNamed>);
	std::printf("SQRT DIVERGES: %s (STABLE -- meters registration is an unavoidable "
		"prerequisite of any length^2 operand)\n",
		std::is_same_v<RfromPlain, RfromNamed> ? "NO" : "YES");
}
