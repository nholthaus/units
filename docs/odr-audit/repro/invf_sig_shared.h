// ODR-AUDIT REPRO invf_sig -- SOURCE: operator/(T lhs, const UnitTypeRhs&) INVERSE/SCALAR (core.h ~4192)
//                              SURFACE: the divergent type embedded in a STRUCT FIELD and a FUNCTION SIGNATURE
//
// SOURCE under test: `1.0 / units::seconds<double>{}`. Result type diverges by TU (typeid-proven):
//   WITH <units/frequency.h> -> units::frequency::hertz<double>;
//   WITHOUT it              -> the plain inverse-time units::unit<...>.
//
// SURFACE: instead of reading the type through a value/text (s01/s02) or serializing it
//   (invf_serial), here the divergent type is baked into an ENTITY'S TYPE ITSELF:
//     (1) a struct field  `struct Holder { decltype(1.0/seconds) v; };`
//     (2) a function signature `int consume_named(decltype(1.0/seconds) x)` -- a weak inline whose
//         parameter type is the divergent type.
//
// TWO DISTINCT ODR OUTCOMES, both demonstrated by this repro (see run.sh notes / OBSERVED below):
//
//   (A) EXPOSED signature  ->  DIVERGES-type-only (different MANGLED symbol per TU).
//       consume_named()'s mangled name embeds its parameter type, so the two TUs emit DIFFERENT
//       symbols:
//         WITH freq:    _Z12consume_namedN5units9frequency5hertzIdEE
//         WITHOUT freq: _Z12consume_namedN5units4unitI...time...NS_12linear_scaleEEE
//       They do not even collide, so the linker keeps BOTH -- but the weak inline that was meant to
//       be ONE entity has SPLIT into two distinct functions. Any header that declares a function or
//       API in terms of `decltype(1.0/seconds)` inherits this split: a caller in a freq-visible TU
//       and a callee compiled in a freq-blind TU resolve to different symbols, i.e. the classic
//       "one declaration, two incompatible definitions" ODR violation on the TYPE/SIGNATURE. The
//       repro proves the split by dumping both symbols with `nm` (see run.sh).
//
//   (B) HIDDEN body (fixed signature)  ->  DIVERGES-type-only, BENIGN-SAME-LAYOUT (real collision,
//       no miscompile). layout_probe(double) has a FIXED mangled name `_Z12layout_probed` in both
//       TUs -- a REAL weak-symbol collision; the linker keeps ONE. Its body builds Holder internally.
//       Because hertz<double> is `struct hertz : unit<...>` with NO added data members (a trivial
//       derived type), sizeof(Holder) is IDENTICAL (8) in both TUs and the value is identical, so the
//       kept definition behaves exactly like the discarded one: the ODR violation is real but has NO
//       observable effect. This is the important negative result -- the rewrap ODR is benign HERE
//       precisely because named units add no state; a named type that added a member (or changed
//       alignment) would make this a genuine layout/miscompile hazard.
//
// BUILD (both link orders + symbol dump; see run.sh):
//   g++ -std=c++23 -I /e/workspace/units/include -c invf_sig_a.cpp invf_sig_b.cpp invf_sig_main.cpp
//   nm -C invf_sig_a.o | grep consume_named     # hertz signature
//   nm -C invf_sig_b.o | grep consume_named     # plain unit<...> signature  -> DIFFERENT mangling
//   g++ invf_sig_a.o invf_sig_b.o invf_sig_main.o -o invf_sig_ab   # layout_probe: with-freq kept
//   g++ invf_sig_b.o invf_sig_a.o invf_sig_main.o -o invf_sig_ba   # layout_probe: without-freq kept
//   ./invf_sig_ab ; ./invf_sig_ba
//
// OBSERVED (g++ 15, clang 20, -std=c++23):
//   consume_named mangled DIFFERENTLY per TU (symbol split confirmed).
//   layout_probe(2): sizeof(Holder)=8 and value=0 in BOTH TUs and BOTH link orders -> prints
//     "size=8 val=0" identically. VERDICT: DIVERGES-type-only (signature split; body benign-same-layout).
#pragma once
#include <units/time.h>

// (A) EXPOSED SIGNATURE: parameter type IS the divergent type. Weak inline; its mangled name embeds
//     whichever type decltype(1.0/seconds) is in the compiling TU.
inline int consume_named(decltype(1.0 / units::seconds<double>(1)) x)
{
	return static_cast<int>(x.value());
}

// (B) HIDDEN BODY, FIXED SIGNATURE: same mangled name in every TU (a real collision). Reports the
//     struct-field layout and the constructed value so the caller can compare across TUs/link orders.
inline unsigned long layout_probe(double v)
{
	struct Holder
	{
		decltype(1.0 / units::seconds<double>(1)) x; // struct FIELD of the divergent type
	};
	Holder h{1.0 / units::seconds<double>(v)};
	// pack size and value so one number carries both facts: size*1000 + value
	return static_cast<unsigned long>(sizeof(Holder)) * 1000 + static_cast<unsigned long>(h.x.value());
}
