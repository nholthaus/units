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
  2. a deliberate ill-formed use names the FRIENDLY unit type (meters<double>, hertz<double>,
     square_meters<int>, ...) and never the raw conversion_factor<...> template soup.

Hand-written cases (357_ordering.cpp, readable_*.cpp) are left in place; this only (re)writes the
generated_*.cpp files, so re-running is idempotent and won't clobber the curated ones.
"""
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
CASES = HERE / "cases"
SOUP = "conversion_factor<std::ratio<1>, units::dimension_t"  # the thing readability must never show

# (name, headers, using, body-lines, expect, [expect-match...], [forbid-match...])
BAD_CONVERSIONS = [
    # deliberate cross-dimension assignment -> must name BOTH friendly types, never soup
    ("length_to_time", ["length", "time"], "units::time::seconds<double> x = 1.0_m;",
     ["meters<double>", "seconds<double>"]),
    ("velocity_to_length", ["velocity", "length"], "units::length::meters<double> x = 1.0_mps;",
     ["meters_per_second<double>", "meters<double>"]),
    ("mass_to_force", ["mass", "force"], "units::force::newtons<double> x = 1.0_kg;",
     ["kilograms<double>", "newtons<double>"]),
    ("area_to_length", ["area", "length"], "units::length::meters<double> x = units::area::square_meters<double>(4.0);",
     ["square_meters<double>", "meters<double>"]),
    ("frequency_to_time", ["frequency", "time"], "units::time::seconds<double> x = units::frequency::hertz<double>(2.0);",
     ["hertz<double>", "seconds<double>"]),
    ("angle_to_length", ["angle", "length"], "units::length::meters<double> x = units::angle::radians<double>(1.0);",
     ["radians<double>", "meters<double>"]),
    ("energy_to_power", ["energy", "power"], "units::power::watts<double> x = units::energy::joules<double>(3.0);",
     ["joules<double>", "watts<double>"]),
    ("pressure_to_force", ["pressure", "force"], "units::force::newtons<double> x = units::pressure::pascals<double>(5.0);",
     ["pascals<double>", "newtons<double>"]),
    ("temperature_to_time", ["temperature", "time"], "units::time::seconds<double> x = units::temperature::kelvin<double>(300.0);",
     ["kelvin<double>", "seconds<double>"]),
    ("charge_to_current", ["charge", "current"], "units::current::amperes<double> x = units::charge::coulombs<double>(1.0);",
     ["coulombs<double>", "amperes<double>"]),
    ("volume_to_area", ["volume", "area"], "units::area::square_meters<double> x = units::volume::cubic_meters<double>(1.0);",
     ["cubic_meters<double>", "square_meters<double>"]),
    ("data_to_time", ["data", "time"], "units::time::seconds<double> x = units::data::bytes<double>(8.0);",
     ["bytes<double>", "seconds<double>"]),
]

ADD_INCOMPATIBLE = [
    ("add_length_time", ["length", "time"], "auto bad = 1.0_m + 1.0_s;", ["meters<double>", "seconds<double>"]),
    ("add_mass_length", ["mass", "length"], "auto bad = 1.0_kg + 1.0_m;", ["kilograms<double>", "meters<double>"]),
    ("sub_velocity_area", ["velocity", "area"], "auto bad = 1.0_mps - units::area::square_meters<double>(1.0);",
     ["meters_per_second<double>", "square_meters<double>"]),
    ("add_frequency_angle", ["frequency", "angle"], "auto bad = units::frequency::hertz<double>(1.0) + units::angle::radians<double>(1.0);",
     ["hertz<double>", "radians<double>"]),
]

# derived-result cases: forming the product/quotient must name the DERIVED friendly type when assigned wrong
DERIVED_RESULT = [
    ("mul_length_length_to_time", ["length", "time", "area"],
     "units::time::seconds<double> x = 2.0_m * 2.0_m;", ["square_meters<double>", "seconds<double>"]),
    ("div_length_time_to_mass", ["length", "time", "velocity", "mass"],
     "units::mass::kilograms<double> x = 10.0_m / 2.0_s;", ["meters_per_second<double>", "kilograms<double>"]),
    ("div_energy_time_to_length", ["energy", "time", "power", "length"],
     "units::length::meters<double> x = units::energy::joules<double>(6.0) / 2.0_s;", ["watts<double>", "meters<double>"]),
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
    return "\n".join(f"#include <units/{h}.h>" for h in hdrs)

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
    for c in ORDERING_OK:
        gen_ordering(c[0], c[1], c[2], c[3])
    total = len(BAD_CONVERSIONS) + len(ADD_INCOMPATIBLE) + len(DERIVED_RESULT) + len(ORDERING_OK)
    print(f"generated {total} cases (+ 4 curated hand-written) = {total + 4} total")

if __name__ == "__main__":
    main()
