// ODR-AUDIT REPRO invf_ctype -- SOURCE: operator/(T lhs, const UnitTypeRhs&) INVERSE/SCALAR (core.h ~4192)
//                                SURFACE: overload resolution keyed off decltype(1.0/seconds)
//
// SOURCE under test: `1.0 / units::seconds<double>{}`. Result type diverges by TU (typeid-proven):
//   WITH <units/frequency.h> -> units::frequency::hertz<double>;
//   WITHOUT it              -> the plain inverse-time units::unit<conversion_factor<ratio<1>,
//                              dimension_t<dim<time_tag, ratio<-1>>>>, double, linear_scale>.
//   hertz<double> is a DISTINCT named class (struct hertz : unit<...>) that the UNIT_ADD macro
//   emits only when frequency.h is included.
//
// SURFACE: a real, constrained OVERLOAD SET. handle() has two overloads distinguished by a
//   requires-clause on `is_hertz<T>`; the weak inline pick_inv() calls handle(1.0/seconds), so
//   OVERLOAD RESOLUTION picks the named overload (code 1) in a freq-visible TU and the plain
//   overload (code 2) in a freq-blind TU. pick_inv() is a weak inline, so the linker keeps ONE
//   definition and the SELECTED FUNCTION -- the actual call target compiled into the binary --
//   FLIPS 1<->2 by LINK ORDER. This is a value/behavior divergence: the program dispatches to a
//   different function depending on link order.
//
// The named class is only FORWARD-DECLARED here (enough to be the is_hertz specialization key),
//   so this header does NOT pull frequency's UNIT_ADD registration; each .cpp controls visibility
//   by whether it includes <units/frequency.h> before this header (the s02 technique). Using the
//   type only as a trait key means it need not be COMPLETE, so the overload set is always
//   well-formed and the sole variable is which overload decltype(1.0/seconds) resolves to.
//
// std::common_type note: the same divergence propagates through std::common_type_t --
//   std::common_type_t<decltype(1.0/seconds), hertz<double>> is hertz<double> in a freq-visible TU
//   and the plain unit<...> otherwise -- so any generic code selecting on a common type inherits
//   this flip. The constrained-overload form above is the sharpest observable demonstration.
//
// BUILD (both link orders):
//   g++ -std=c++23 -I /e/workspace/units/include -c invf_ctype_a.cpp invf_ctype_b.cpp invf_ctype_main.cpp
//   g++ invf_ctype_a.o invf_ctype_b.o invf_ctype_main.o -o invf_ctype_ab   # a (with freq) kept -> 1
//   g++ invf_ctype_b.o invf_ctype_a.o invf_ctype_main.o -o invf_ctype_ba   # b (without freq) kept -> 2
//   ./invf_ctype_ab ; ./invf_ctype_ba
//
// OBSERVED (g++ 15, clang 20, -std=c++23): AB prints "a=1 b=1", BA prints "a=2 b=2" (both compilers).
//   VERDICT: DIVERGES-observably.
#pragma once
#include <units/time.h>
#include <type_traits>

// Forward-declare the named class so is_hertz below has a specialization key WITHOUT pulling the
// UNIT_ADD registration (mirrors the s02 forward-declaration technique).
namespace units { inline namespace frequency { template<class> struct hertz; } }

// Trait: is this the named hertz<double>? A type key only -- hertz need not be complete.
template<class T>
struct is_hertz : std::false_type
{
};
template<>
struct is_hertz<units::frequency::hertz<double>> : std::true_type
{
};

// A genuine constrained overload set; the requires-clause selects exactly one at each call.
template<class T>
	requires(is_hertz<T>::value)
inline int handle(const T&)
{
	return 1; // named overload chosen when the argument's static type IS hertz<double>
}
template<class T>
	requires(!is_hertz<T>::value)
inline int handle(const T&)
{
	return 2; // plain overload chosen otherwise
}

// Weak inline whose overload selection depends on decltype(1.0/seconds) in the current TU.
inline int pick_inv()
{
	return handle(1.0 / units::seconds<double>(1));
}
