// ODR-AUDIT REPRO divc_serial -- SOURCE: operator/ COMPOUND cross-dimension (core.h ~4091)
//                                 SURFACE: serialization (wire bytes + any_unit::to_string())
//
// SOURCE under test: `units::meters<double>{} / units::seconds<double>{}` (the s01/s02 anchor's
//   source, on a NEW surface). Its RESULT TYPE diverges by TU: WITH <units/velocity.h> it is
//   units::velocity::meters_per_second<double>; WITHOUT it, the plain
//   units::unit<conversion_factor<ratio<1>, dimension_t<dim<length_tag,1>, dim<time_tag,-1>>>,
//   double, linear_scale>. So the weak inline serialize_mps() is compiled with two DIFFERENT
//   fed-in types across the two .cpp.
//
// QUESTION: does that divergence reach the serialized form? Same analysis as invf_serial:
//   serialize() keys off the base-dimension signature (length^1 time^-1) + SI-base magnitude,
//   and to_string() resolves the canonical unit for that dimension via visit(). Both are
//   invariant to whether the compile-time type was the named velocity or the plain compound.
//   PREDICTION: wire bytes STABLE, to_string() STABLE.
//
// BUILD (both link orders):
//   g++ -std=c++23 -I /e/workspace/units/include -c divc_serial_a.cpp divc_serial_b.cpp divc_serial_main.cpp
//   g++ divc_serial_a.o divc_serial_b.o divc_serial_main.o -o divc_serial_ab   # a (with velocity) kept
//   g++ divc_serial_b.o divc_serial_a.o divc_serial_main.o -o divc_serial_ba   # b (without velocity) kept
//   ./divc_serial_ab ; ./divc_serial_ba   # EXPECT identical hex + identical to_string in both
//
// OBSERVED (g++ 15, clang 20): both orders print, for BOTH a() and b(),
//   bytes = 01 01 02 <8-byte length-hash> 01 <8-byte time-hash> ... value(2.5)
//   to_string = "2.5 m s^-1"   => wire form STABLE, to_string() STABLE. VERDICT: STABLE.
//   (Contrast s01: the SAME 2.5 m/s rendered "mps" vs "m s^-1" by link order when the SURFACE
//    called units::to_string() on the NAMED type directly. Here the surface goes through the
//    erased any_unit, whose text is dimension-resolved, so the named-type divergence is erased.)
#pragma once
#include <units/length.h>
#include <units/time.h>
#include <units/serialization.h>
#include <string>
#include <vector>
#include <cstdio>

struct SerRecord
{
	std::vector<unsigned char> bytes;
	std::string                text;
};

inline SerRecord serialize_mps()
{
	// 5 m / 2 s = 2.5 m/s in SI base
	units::any_unit a = units::serialize(units::meters<double>(5) / units::seconds<double>(2));
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
