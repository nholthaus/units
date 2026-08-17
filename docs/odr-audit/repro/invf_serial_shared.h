// ODR-AUDIT REPRO invf_serial -- SOURCE: operator/(T lhs, const UnitTypeRhs&) INVERSE/SCALAR (core.h ~4192)
//                                 SURFACE: serialization (wire bytes + any_unit::to_string())
//
// SOURCE under test: `1.0 / units::seconds<double>{}`. A typeid probe proves its RESULT TYPE
//   diverges by TU: WITH <units/frequency.h> it is units::frequency::hertz<double>; WITHOUT it,
//   the plain units::unit<conversion_factor<ratio<1>, dimension_t<dim<time_tag, ratio<-1>>>>,
//   double, linear_scale>. So the weak inline serialize_inv() below is compiled with two
//   DIFFERENT return-expression types across the two .cpp -- the classic rewrap ODR setup.
//
// QUESTION (the empirical claim to PROVE or REFUTE): does that type divergence leak into the
//   SERIALIZED FORM? i.e.
//     (1) do the produced wire BYTES differ by TU / by link order?
//     (2) does any_unit::to_string() differ by TU / by link order?
//
// READING OF serialization.h (to be proven empirically here):
//   * serialize<Unit>() computes signature<Unit>::value -- the BASE-DIMENSION decomposition
//     (each base dim by 8-byte name-hash + rational exponent) -- and the magnitude in the SI
//     CANONICAL BASE unit of the dimension (canonical_unit_t<Dim>). hertz<double> and the plain
//     inverse-time unit<...> share the SAME conversion_factor dimension (time^-1) and the SAME
//     base value, so both encode() to the SAME bytes.
//   * any_unit::to_string() renders via visit() over builtin_dimensions, selecting the canonical
//     unit by DIMENSION-SIGNATURE match -- never by the named source type. So the rendering is
//     also independent of hertz-vs-plain.
//   => PREDICTION: wire bytes STABLE, to_string() STABLE. This repro proves it: the weak inline
//      is defined in a freq-visible TU and a freq-blind TU, and BOTH link orders produce IDENTICAL
//      bytes and IDENTICAL text. (Contrast s01/s02, where the SURFACE read the named type directly
//      and DID diverge.)
//
// BUILD (both link orders; see run at bottom):
//   g++ -std=c++23 -I /e/workspace/units/include -c invf_serial_a.cpp invf_serial_b.cpp invf_serial_main.cpp
//   g++ invf_serial_a.o invf_serial_b.o invf_serial_main.o -o invf_serial_ab   # a (with freq) kept
//   g++ invf_serial_b.o invf_serial_a.o invf_serial_main.o -o invf_serial_ba   # b (without freq) kept
//   ./invf_serial_ab ; ./invf_serial_ba   # EXPECT identical hex + identical to_string in both
//
// OBSERVED (g++ 15, clang 20, -std=c++23): both orders print
//   bytes = 01 01 01 5a 48 05 56 68 18 60 00 01 00 00 00 3f   to_string = "0.5 s^-1"
//   for BOTH a() and b(). => wire form STABLE, to_string() STABLE. VERDICT: STABLE.
#pragma once
#include <units/time.h>
#include <units/serialization.h>
#include <string>
#include <vector>
#include <cstdio>

// Whichever definition of this weak inline the linker keeps, its RETURN TYPE (any_unit) is the
// same, but the type FED to serialize() differs by TU (hertz<double> vs plain unit<...>). We
// return a printable record of BOTH the bytes and the text so the caller can compare across TUs.
struct SerRecord
{
	std::vector<unsigned char> bytes;
	std::string                text;
};

inline SerRecord serialize_inv()
{
	// value 0.5 in SI base (1 / 2 s)
	units::any_unit a = units::serialize(1.0 / units::seconds<double>(2));
	SerRecord r;
	r.bytes.assign(reinterpret_cast<const unsigned char*>(a.data()),
	               reinterpret_cast<const unsigned char*>(a.data()) + a.size());
	r.text = a.to_string();
	return r;
}

inline std::string to_hex(const std::vector<unsigned char>& b)
{
	std::string out;
	char        buf[4];
	for (std::size_t i = 0; i < b.size(); ++i)
	{
		std::snprintf(buf, sizeof(buf), "%02x", b[i]);
		if (i)
			out += ' ';
		out += buf;
	}
	return out;
}
