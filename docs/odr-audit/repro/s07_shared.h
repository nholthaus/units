// ODR-AUDIT REPRO s07 -- SOURCE: operator/ (m/s) x SURFACE: struct-field / function-signature
//
// EXPECTED: the type embedded in Holder.v (and the mangled signature of make_holder())
//   is TU-invariant, so the one weak inline make_holder() has one definition.
// OBSERVED: Holder wraps decltype(m/s), which is the NAMED type in a TU that saw the
//   registration and the plain unit<...> base in one that did not. The weak inline
//   make_holder() returns Holder; its MANGLED SIGNATURE therefore differs between TUs
//   (the field type differs), so the two .o files export two different-but-same-named
//   weak symbols. This is an ODR violation on a TYPE even though (for m/s) the layout
//   is identical (size 8), so it does NOT miscompile the value -- the harm is purely
//   type-identity nondeterminism. We demonstrate the divergence by (1) the mangled
//   name difference in the two objects and (2) that .value() is unchanged (2.5).
#pragma once
#include <units/length.h>
#include <units/time.h>
struct Holder { decltype(units::meters<double>(1) / units::seconds<double>(1)) v; };
inline Holder make_holder(double m, double s) { return Holder{ units::meters<double>(m) / units::seconds<double>(s) }; }
inline double holder_value(double m, double s) { return make_holder(m, s).v.value(); }
