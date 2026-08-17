#!/usr/bin/env python3
"""
Generate the error-message harness case corpus for nholthaus/units.

Each generated cases/*.cpp carries in-file directives the runner (run.py) grades:
  // expect: pass|fail
  // expect-match: <substr>     the diagnostic MUST contain this (readable named type present)
  // forbid-match: <substr>     the diagnostic must NOT contain this (no conversion_factor soup)

The corpus proves two things across the unit zoo:
  1. #357-class include-ordering never regresses (an expression reducing to a not-yet-included
     dimension still compiles);
  2. a deliberate ill-formed use names the FRIENDLY unit type (asserted as `meters<`, `hertz<`,
     `square_meters<`, ...) and never the raw conversion_factor<...> template soup. The `name<` token
     matches the template form on every compiler — g++'s `meters<double>` and the default-elided
     `meters<>` of clang/MSVC — while rejecting the `meters_` conversion-factor tag and the plain
     `unit<...>` base, so a soup regression fails the assertion.

Hand-written cases (357_ordering.cpp, readable_*.cpp) are left in place; this only (re)writes the
generated_*.cpp files, so re-running is idempotent and won't clobber the curated ones.
"""
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
CASES = HERE / "cases"
SOUP = "conversion_factor<std::ratio<1>, units::dimension_t"  # the thing readability must never show
# A second soup marker: the friendly name must not be drowned in a `dimension_t<...>` wall either. Forbidding
# both proves the diagnostic names the type/operator WITHOUT descending into the template internals.
SOUP_DIMENSION = "dimension_t<"

# (name, headers, using, body-lines, expect, [expect-match...], [forbid-match...])
BAD_CONVERSIONS = [
    # deliberate cross-dimension assignment -> must name BOTH friendly types, never soup
    ("length_to_time", ["length", "time"], "units::time::seconds<double> x = 1.0_m;",
     ["meters<", "seconds<"]),
    ("velocity_to_length", ["velocity", "length"], "units::length::meters<double> x = 1.0_mps;",
     ["meters_per_second<", "meters<"]),
    ("mass_to_force", ["mass", "force"], "units::force::newtons<double> x = 1.0_kg;",
     ["kilograms<", "newtons<"]),
    ("area_to_length", ["area", "length"], "units::length::meters<double> x = units::area::square_meters<double>(4.0);",
     ["square_meters<", "meters<"]),
    ("frequency_to_time", ["frequency", "time"], "units::time::seconds<double> x = units::frequency::hertz<double>(2.0);",
     ["hertz<", "seconds<"]),
    ("angle_to_length", ["angle", "length"], "units::length::meters<double> x = units::angle::radians<double>(1.0);",
     ["radians<", "meters<"]),
    ("energy_to_power", ["energy", "power"], "units::power::watts<double> x = units::energy::joules<double>(3.0);",
     ["joules<", "watts<"]),
    ("pressure_to_force", ["pressure", "force"], "units::force::newtons<double> x = units::pressure::pascals<double>(5.0);",
     ["pascals<", "newtons<"]),
    ("temperature_to_time", ["temperature", "time"], "units::time::seconds<double> x = units::temperature::kelvin<double>(300.0);",
     ["kelvin<", "seconds<"]),
    ("charge_to_current", ["charge", "current"], "units::current::amperes<double> x = units::charge::coulombs<double>(1.0);",
     ["coulombs<", "amperes<"]),
    ("volume_to_area", ["volume", "area"], "units::area::square_meters<double> x = units::volume::cubic_meters<double>(1.0);",
     ["cubic_meters<", "square_meters<"]),
    ("data_to_time", ["data", "time"], "units::time::seconds<double> x = units::data::bytes<double>(8.0);",
     ["bytes<", "seconds<"]),
]

ADD_INCOMPATIBLE = [
    ("add_length_time", ["length", "time"], "auto bad = 1.0_m + 1.0_s;", ["meters<", "seconds<"]),
    ("add_mass_length", ["mass", "length"], "auto bad = 1.0_kg + 1.0_m;", ["kilograms<", "meters<"]),
    ("sub_velocity_area", ["velocity", "area"], "auto bad = 1.0_mps - units::area::square_meters<double>(1.0);",
     ["meters_per_second<", "square_meters<"]),
    ("add_frequency_angle", ["frequency", "angle"], "auto bad = units::frequency::hertz<double>(1.0) + units::angle::radians<double>(1.0);",
     ["hertz<", "radians<"]),
]

# derived-result cases: forming the product/quotient must name the DERIVED friendly type when assigned wrong
DERIVED_RESULT = [
    ("mul_length_length_to_time", ["length", "time", "area"],
     "units::time::seconds<double> x = 2.0_m * 2.0_m;", ["square_meters<", "seconds<"]),
    ("div_length_time_to_mass", ["length", "time", "velocity", "mass"],
     "units::mass::kilograms<double> x = 10.0_m / 2.0_s;", ["meters_per_second<", "kilograms<"]),
    ("div_energy_time_to_length", ["energy", "time", "power", "length"],
     "units::length::meters<double> x = units::energy::joules<double>(6.0) / 2.0_s;", ["watts<", "meters<"]),
]

# A dimensioned quantity does not implicitly become a bare scalar, and a bare number does not implicitly
# become a dimensioned quantity: the conversion operator is explicit and the value constructor is explicit.
# The `meters<` token matches the friendly template form (`meters<double>` on g++, default-elided `meters<>`
# on clang/MSVC) while rejecting the `meters_` conversion-factor tag and the plain `unit<...>` base, so a
# soup regression fails the assertion.
SCALAR_BOUNDARY = [
    ("scalar_from_dimensioned", ["length"], "double d = 1.0_m;", ["meters<"]),
    ("dimensioned_from_scalar", ["length"], "units::length::meters<double> m = 5.0;", ["meters<"]),
]

# Comparing quantities of different dimensions is ill-formed. The relational-operator diagnostic reports the
# operands through their conversion-factor tag inside the `unit<...>` base (e.g. `unit<units::meters_>`), so it
# does not spell the friendly `meters<double>` form on any compiler. Assert what IS present and is not soup: the
# failing operator name, which every compiler surfaces in the instantiation context of the rejected relational
# overload. The operator token is per-compiler because the SPELLING differs — g++/clang write it tight
# (`operator<`), MSVC writes a space (`operator <`) — so it is emitted as expect-match-gcc / expect-match-msvc.
# A friendly type token (`kilograms<`, `meters<`, a bare stem, ...) is NOT asserted here: it appears in a
# compiler's output only where that compiler happens to echo the offending SOURCE line — which g++-13 omits and
# g++-15/clang include — so matching it grades the source echo, not the diagnostic, and is fragile across
# compiler versions. Readability is verified two-sided: the operator IS named AND the message is not buried in
# conversion-factor / dimension soup (both forbid tokens confirmed absent on GCC-13/15, clang, and MSVC).
# Entry shape: (name, headers, body, operator-symbol).
COMPARE_ACROSS = [
    ("compare_length_time_gt", ["length", "time"], "bool b = (1.0_m > 1.0_s);", ">"),
    ("compare_velocity_mass_ge", ["velocity", "mass"],
     "bool b = (1.0_mps >= units::mass::kilograms<double>(1.0));", ">="),
]

# A math function whose domain is an angle rejects a non-angle argument. The `name<` token matches the
# friendly template form (`meters<double>` on g++, default-elided `meters<>` on clang/MSVC) while rejecting
# the conversion-factor tag and the plain `unit<...>` base.
TRIG_DOMAIN = [
    ("sin_of_length", ["angle", "length"], "auto x = sin(1.0_m);", ["meters<"]),
    ("cos_of_time", ["angle", "time"], "auto x = cos(units::time::seconds<double>(1.0));", ["seconds<"]),
    ("tan_of_mass", ["angle", "mass"], "auto x = tan(units::mass::kilograms<double>(1.0));", ["kilograms<"]),
]

# The RESULT of a dimensional math operation has a definite dimension; assigning it to the wrong one
# fails, and the diagnostic names the RESULT's friendly type (sqrt of an area is a length; the square of a
# length is an area) alongside the wrong target — like the mul/div derived-result cases above.
MATH_RESULT = [
    ("sqrt_area_to_time", ["area", "length", "time"],
     "units::time::seconds<double> x = sqrt(units::area::square_meters<double>(4.0));",
     ["meters<", "seconds<"]),
    ("pow2_length_to_volume", ["length", "area", "volume"],
     "units::volume::cubic_meters<double> x = pow<2>(1.0_m);", ["square_meters<", "cubic_meters<"]),
]

# fmod and hypot across incompatible dimensions are ill-formed. The `name<` token matches the friendly
# template form while rejecting the conversion-factor tag and the plain `unit<...>` base.
MATH_DOMAIN = [
    ("fmod_length_time", ["length", "time"], "auto x = fmod(1.0_m, 1.0_s);", ["meters<", "seconds<"]),
]

# A std::chrono::duration only converts to/from a time quantity; a non-time quantity is rejected. The
# `meters<` token matches the friendly template form while rejecting the `meters_` tag and the `unit<...>` base.
CHRONO_BOUNDARY = [
    ("chrono_from_length", ["length", "<chrono>"], "std::chrono::seconds s = 1.0_m;", ["meters<"]),
]

# #357-class ordering: an expression reducing to a dimension whose header is included LAST must still compile.
ORDERING_OK = [
    ("order_velocity_over_length", ["velocity", "length"], "auto x = 1.0_mps / 1.0_m;", "frequency"),
    ("order_length_over_time", ["length", "time"], "auto x = 1.0_m / 1.0_s;", "velocity"),
    ("order_area_over_length", ["area", "length"], "auto x = units::area::square_meters<double>(4.0) / 1.0_m;", "length"),
    ("order_length_times_length", ["length"], "auto x = 1.0_m * 1.0_m;", "area"),
    ("order_energy_over_time", ["energy", "time"], "auto x = units::energy::joules<double>(6.0) / 1.0_s;", "power"),
    ("order_mass_times_accel", ["mass", "acceleration"],
     "auto x = 1.0_kg * units::acceleration::meters_per_second_squared<double>(1.0);", "force"),
]

def header_includes(hdrs):
    # a bare "<name>" is a standard-library header included verbatim; otherwise it is a units dimension header
    def one(h):
        return f"#include {h}" if h.startswith("<") else f"#include <units/{h}.h>"
    return "\n".join(one(h) for h in hdrs)

def write(name, text):
    (CASES / f"generated_{name}.cpp").write_text(text)

def gen_bad_conversion(name, hdrs, body, matches):
    directives = ["// expect: fail"] + [f"// expect-match: {m}" for m in matches] + [f"// forbid-match: {SOUP}"]
    txt = f"""// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension use — the diagnostic must name the
// FRIENDLY unit types, never the raw conversion_factor<...> soup.
{chr(10).join(directives)}
{header_includes(hdrs)}
using namespace units;
using namespace units::literals;
{body}
int main() {{ return 0; }}
"""
    write(name, txt)

def gen_compare(name, hdrs, body, op):
    # A cross-dimension relational compare: the readable signal is the failing operator, asserted per-compiler
    # because g++/clang spell it tight (`operator>`) and MSVC inserts a space (`operator >`). Two forbid guards
    # confirm the message is not buried in conversion-factor / dimension soup.
    directives = ["// expect: fail",
                  f"// expect-match-gcc: operator{op}",
                  f"// expect-match-msvc: operator {op}",
                  f"// forbid-match: {SOUP}",
                  f"// forbid-match: {SOUP_DIMENSION}"]
    txt = f"""// GENERATED (generate_cases.py). Deliberate ill-formed cross-dimension comparison — the diagnostic must name
// the failing operator, never the raw conversion_factor<...> / dimension_t<...> soup.
{chr(10).join(directives)}
{header_includes(hdrs)}
using namespace units;
using namespace units::literals;
{body}
int main() {{ return 0; }}
"""
    write(name, txt)

def gen_ordering(name, hdrs, body, last_dim):
    # include the reduced dimension's header LAST, after the expression is formed, to exercise #357.
    txt = f"""// GENERATED (generate_cases.py). #357-class ordering: an expression reducing to the '{last_dim}' dimension is
// formed BEFORE that dimension's header is included — must still compile (no explicit-specialization-after-
// instantiation).
// expect: pass
{header_includes(hdrs)}
using namespace units;
using namespace units::literals;
{body}
#include <units/{last_dim}.h>
int main() {{ (void)sizeof(x); return 0; }}
"""
    write(name, txt)

def main():
    # clear old generated files so removals don't linger
    for f in CASES.glob("generated_*.cpp"):
        f.unlink()
    for c in BAD_CONVERSIONS:
        gen_bad_conversion(c[0], c[1], c[2], c[3])
    for c in ADD_INCOMPATIBLE:
        gen_bad_conversion(c[0], c[1], c[2], c[3])
    for c in DERIVED_RESULT:
        gen_bad_conversion(c[0], c[1], c[2], c[3])
    for group in (SCALAR_BOUNDARY, TRIG_DOMAIN, MATH_RESULT, MATH_DOMAIN, CHRONO_BOUNDARY):
        for c in group:
            gen_bad_conversion(c[0], c[1], c[2], c[3])
    for c in COMPARE_ACROSS:
        gen_compare(c[0], c[1], c[2], c[3])
    for c in ORDERING_OK:
        gen_ordering(c[0], c[1], c[2], c[3])
    total = (len(BAD_CONVERSIONS) + len(ADD_INCOMPATIBLE) + len(DERIVED_RESULT) + len(SCALAR_BOUNDARY)
             + len(COMPARE_ACROSS) + len(TRIG_DOMAIN) + len(MATH_RESULT) + len(MATH_DOMAIN)
             + len(CHRONO_BOUNDARY) + len(ORDERING_OK))
    print(f"generated {total} cases (+ 4 curated hand-written) = {total + 4} total")

if __name__ == "__main__":
    main()
