# units

A compile-time, header-only, dimensional-analysis and unit-conversion library for **C++23**, with no
dependencies.

![Windows build](https://github.com/nholthaus/units/actions/workflows/msvc-2022.yaml/badge.svg)
![GCC build](https://github.com/nholthaus/units/actions/workflows/gcc-13.yaml/badge.svg)
![Clang build](https://github.com/nholthaus/units/actions/workflows/clang-19.yaml/badge.svg)
![license](https://img.shields.io/badge/license-MIT-orange.svg)
![copyright](https://img.shields.io/badge/%C2%A9-Nic_Holthaus-orange.svg)
![standard](https://img.shields.io/badge/std-c%2B%2B23-blue.svg)

`units` represents physical quantities as types. A quantity is a value with a unit — `meters`, `feet`,
`seconds` — that behaves like the number it wraps. Conversions between compatible units are implicit and
resolved at compile time; expressions that are dimensionally inconsistent do not compile.

Quantities are written with unit literals (`5.0_m`) or by multiplying a value by a unit constant
(`60.0 * km`):

```cpp
#include <units/length.h>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

    meters a = 5.0_m;        // unit literal
    meters b = 60.0 * km;    // value times a unit constant (== 60000 m)
    feet   c = a;            // implicit, lossless conversion

    std::cout << a << ", " << b << ", " << c << '\n';   // prints: 5 m, 60000 m, 16.4042 ft
}
```

`units` favors syntax that reads as ordinary code: quantities are written and combined the way you would
write them by hand, so the common cases are apparent from the code without consulting the reference.

Every snippet in this README and in the [documentation](docs/) is compiled and run as part of the test
suite — see [`examples/`](examples/).

## Design

The library is organized around syntax that reads as ordinary arithmetic. A quantity is constructed with
a unit literal (`5.0_m`) or a unit constant (`60.0 * km`), combined with the usual operators (`+`, `*`,
`/`, comparisons), converted by assignment, and printed with `<<`. The common operations are intended to
work as written; the deeper machinery (class-based named types, CTAD, ADL) exists so that this surface
stays small and the code stays legible.

## Contents

- [Design](#design)
- [Features](#features)
- [Requirements](#requirements)
- [Getting started](#getting-started)
- [Type errors](#type-errors)
- [Run-time cost](#run-time-cost)
- [Integration](#integration)
- [Cheat sheet](#cheat-sheet)
- [Supported units](#supported-units)
- [Physical constants](#physical-constants)
- [More capabilities](#more-capabilities)
- [Documentation](#documentation)
  - [Learn](#learn) · [Explain](#explain) · [How-to](#how-to) · [Reference](#reference) · [Meta](#meta)
- [License](#license)

---

## Features

- **Syntax.** Quantities are written as `meters`, `60_mi / 1_hr`, `sqrt(area)`; operations are performed
  on the quantity types.
- **Run-time cost.** Conversions are `constexpr` ratios; a conversion between equivalent representations
  compiles to no machine code. A quantity is a trivially-copyable value the size of its underlying type.
- **Dimensional checking.** Adding a length to a time, or assigning an area to a length, is a compile
  error. The dimensional analysis is performed by the type system.
- **Diagnostics.** A dimensional error names the unit type (`meters<double>`) rather than the
  `conversion_factor<...>` template. See [Type errors](#type-errors).
- **Trivial integration.** Header-only, no dependencies, one `#include`. Drop in the headers, or consume
  the CMake package. See [Integration](#integration).

---

## Requirements

`units` requires a **C++23** compiler. It is continuously tested on:

| Compiler            | Version | Platform        |
|---------------------|---------|-----------------|
| GCC (`g++`)         | 13      | Ubuntu (latest) |
| Clang (`clang++`)   | 19      | Ubuntu (latest) |
| MSVC (Visual Studio)| 2022    | Windows (latest)|

Older toolchains are not supported by the 3.x line. The last release for the C++14 era is the 2.x
series (see [Migrating from 2.x](docs/meta/migrate-v2-to-v3.md)).

---

## Getting started

This section covers what most code needs. The [full manual](docs/) has the rest.

**Include a header, and bring in the literal operators.** Include the umbrella header `<units.h>` for
every dimension, or one per-dimension header (`<units/length.h>`, `<units/time.h>`, …) for just the
dimensions you use:

```cpp
#include <units.h>            // everything; or <units/length.h>, <units/velocity.h>, ... for a subset
using namespace units;
using namespace units::literals;   // the _m, _s, _kg, ... literals
```

> **Note — if compiles are slow, include less.** `<units.h>` pulls in all 47 dimensions. The library is
> heavily templated, so a translation unit's compile time scales with how much it instantiates; including
> only the per-dimension headers you use keeps it down. Include the dimension of every quantity you
> *name*, including result dimensions (dividing a length by a time needs `<units/velocity.h>`). Run-time
> behavior and code size are unaffected either way.

**Make a quantity.** Four equivalent forms:

```cpp
meters a(5.0);          // construction (CTAD deduces meters<double>)
meters b = 5.0_m;       // a unit literal
meters c = 5.0 * m;     // a value times a unit constant (units::m)
meters d{5.0};          // braced construction
```

> **Note — write the decimal point for fractional values.** A literal's type follows what you write:
> `5.0_m` is `meters<double>`, but `5_m` is `meters<int>`. Integer-backed quantities do integer
> arithmetic, so `1_m / 2_m` is `0`, whereas `1.0_m / 2.0_m` is `0.5`. Use a decimal point (or write
> `meters<double>`) when you want fractional results.

**Spelling the type: `meters`, `meters<>`, `meters<T>`.** Three ways to name the type:

- `meters<T>` — an explicit representation (`meters<double>`, `meters<float>`, `meters<int>`). Valid as a
  variable, function parameter, return type, or member.
- `meters<>` — the default representation; identical to `meters<double>`. Valid in the same positions.
- `meters` — the bare name deduces the representation from the initializer (CTAD): `meters a(5.0)` is
  `meters<double>`, `meters a(5)` is `meters<int>`. Valid only where an initializer is present to deduce
  from — a local variable. A function parameter, a return type, and a class member have no initializer,
  so they require `meters<>` or `meters<T>`.

```cpp
meters        local(5.0);      // bare name, deduced meters<double>
meters<>      m;               // default representation
meters<float> as_float(5.0f);  // explicit representation
// void f(meters q);           // ill-formed: a parameter has no initializer to deduce from
// meters make();              // ill-formed: a return type has no initializer to deduce from
```

**`auto` vs. an explicit type.** Use `auto` on the left when the right-hand side already states the unit:

```cpp
auto d = 5.0_m;                  // meters
auto speed = 60.0_mi / 1.0_hr;   // a velocity
```

Write the type explicitly on the left when the compiler should confirm the dimensional analysis: naming
the result type makes a mismatch a compile error rather than an accepted `auto` deduction.

```cpp
square_meters     area  = 15.0_m * 5.0_m;    // the result is an area
meters_per_second speed = 100.0_m / 8.0_s;   // the result is a velocity
// meters bad = 15.0_m * 5.0_m;              // ill-formed: the result is an area, not a length
```

**Convert** by assigning between compatible units (implicit, and only when lossless):

```cpp
meters m = 100.0_ft;    // feet -> meters
feet   f = m;           // meters -> feet
```

**Do arithmetic** — the result carries the correct dimension; name it and the compiler checks it:

```cpp
square_meters     area  = 15.0_m * 5.0_m;    // m * m -> area
meters_per_second speed = 60.0_mi / 1.0_hr;   // -> velocity
meters            side  = sqrt(area / 3.0);   // <cmath> functions are unit-aware (found by ADL)
```

**Get a plain number back out** at the boundary with non-`units` code (there is no implicit
quantity → `double`, except for dimensionless quantities):

```cpp
double v = speed.value();     // the value in the quantity's units
double t = speed.to<double>();// cast to a chosen representation
std::cout << speed;           // or print it directly: "26.8224 mps"
```

That is enough for most use cases. How `meters a(5.0)` deduces its type and why `sqrt` needs no
`units::` prefix are covered in [CTAD and ADL](docs/explain/ctad-and-adl-for-humans.md); the full
walkthrough is in [Getting started](docs/learn/getting-started.md).

---

## Type errors

A dimensional mistake that a bare `double` would accept is rejected at compile time, and the diagnostic
names the unit type. The messages below are captured verbatim from GCC 13.

Adding incompatible dimensions:

```text
readable_add_incompatible.cpp:9:18: error: no match for ‘operator+’ (operand types are ‘units::length::meters<double>’ and ‘units::time::seconds<double>’)
    9 | auto bad = 1.0_m + 1.0_s; // ill-formed: cannot add length and time
      |            ~~~~~ ^ ~~~~~
      |            |       |
      |            |       units::time::seconds<double>
      |            units::length::meters<double>
```

Assigning a product to the wrong dimension — `m * m` is an area, not a length. GCC reports the result
through an internal alias with the named type beside it in `{aka …}`:

```text
readable_wrong_result_type.cpp:10:41: error: conversion from ‘units::detail::rewrap_to_named_t<units::unit<units::area::square_meters_, double, units::linear_scale> >’ {aka ‘units::area::square_meters<double>’} to non-scalar type ‘units::length::meters<double>’ requested
   10 | units::length::meters<double> a = 1.0_m * 1.0_m; // ill-formed: m*m is an area, not a length
      |                                   ~~~~~~^~~~~~~
```

The full set of rejected operations, with the diagnostic each produces on GCC, Clang, and MSVC, is in
[Type safety](docs/explain/type-safety.md). The diagnostics there are captured from the compilers by the
test harness.

---

## Run-time cost

A quantity is a trivially-copyable value the size of its underlying type; conversion ratios are
`constexpr`. The type abstraction compiles away: the generated code matches hand-written `double`. The
following disassembly is at `-O2` (`-O3 -march=x86-64-v3` for the loop); GCC 15 and Clang 21 agree.

**A runtime expression compiles to the same instructions.** Computing a distance from a speed in mph and
a time in seconds — the raw version hard-codes the mph → m/s factor, the `units` version carries it in
the types — yields three floating-point instructions either way (a multiply, a divide, a multiply),
differing only in operand order:

```cpp
double         distance_raw  (double mph, double sec)                        { return (mph * 1609.344 / 3600.0) * sec; }
meters<double> distance_units(miles_per_hour<double> v, seconds<double> t)   { return v * t; }
```
```asm
distance_raw:                        distance_units:
    mulsd   .LC0(%rip), %xmm0            mulsd   %xmm1, %xmm0
    divsd   .LC1(%rip), %xmm0            mulsd   .LC2(%rip), %xmm0
    mulsd   %xmm1, %xmm0                 divsd   .LC3(%rip), %xmm0
    ret                                  ret
```

**A conversion between equivalent representations is free.** Passing a `meters` where a `meters` is
wanted is not a cheap conversion — it is *no* conversion:

```cpp
double roundtrip(meters<double> m) { meters<double> copy = m; return copy.value(); }
```
```asm
roundtrip:
    ret                              ; the whole function
```

**A compile-time conversion is done by the compiler.** A conversion of known values folds to a single
constant load — the arithmetic never runs:

```cpp
double speed_limit_mps() { return meters_per_second<double>(65.0_mph).value(); }
```
```asm
speed_limit_mps:
    movsd   .LC0(%rip), %xmm0        ; xmm0 = 29.0576  (65 mph, converted at compile time)
    ret
```

**A hot loop vectorizes the same.** Summing an array of `kilometers` as `meters` produces the identical
instruction stream — including the AVX vectorization — as the raw-`double` loop; and

```cpp
static_assert(1.0_km + 1.0_m == 1001.0_m);   // evaluated at compile time
```

holds with no run-time work. See [Efficiency](docs/explain/efficiency.md) for the full comparison.

---

## Integration

`units` is header-only.

**Copy the headers.** Put `include/` on your include path and compile with C++23 (`-std=c++23` on GCC and
Clang). Nothing to build.

**CMake — `add_subdirectory`.** Vendor the project and link the interface target:

```cmake
add_subdirectory(units)
target_link_libraries(myapp PRIVATE units::units)
```

**CMake — `FetchContent`.** Pull it at configure time:

```cmake
include(FetchContent)
FetchContent_Declare(units
  GIT_REPOSITORY https://github.com/nholthaus/units.git
  GIT_TAG        v3.4.0)
FetchContent_MakeAvailable(units)
target_link_libraries(myapp PRIVATE units::units)
```

**CMake — installed package.** After installing (or from a Linux package), consume it with
`find_package`:

```cmake
find_package(units CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE units::units)
```

**Linux packages.** The build produces Debian (`libunits-dev`), RPM (`units-devel`), and tarball
artifacts via CPack; a PPA is published for Ubuntu.

Linking `units::units` also attaches the [Visual Studio debugger visualizer](natvis/units.natvis) on
MSVC, so a quantity shows as `5 m` in the debugger rather than an opaque object.

> **Not yet available:** vcpkg and Conan ports. Contributions welcome.

---

## Cheat sheet

The everyday API. Every line compiles under C++23; assumes `using namespace units;` and
`using namespace units::literals;`. The full version, with more detail, is
[docs/reference/cheat-sheet.md](docs/reference/cheat-sheet.md).

```cpp
// Make a quantity
meters a(5.0);            // CTAD -> meters<double>
meters b = 5.0_m;         // literal
auto   c = 5.0 * m;       // scalar * unit constant (units::m)
meters<int> f(5);         // explicit integer representation
// 5_m is meters<int>, 5.0_m is meters<double>: write the dot for fractional values.

// Convert (implicit, lossless only)
meters mm = 100.0_ft;     // feet -> meters
feet   ff = mm;           // meters -> feet
// meters<int> x = 1.0_ft;   // ERROR: lossy into an integer representation

// Arithmetic (dimensions are tracked)
square_meters     area  = 15.0_m * 5.0_m;    // m * m -> area
meters_per_second speed = 60.0_mi / 1.0_hr;  // -> velocity
// meters bad = 15.0_m * 5.0_m;   // ERROR: that product is an area, not a length
// auto z = 1.0_m + 1.0_s;        // ERROR: incompatible dimensions
// auto w = 1.0_m + 5.0;          // ERROR: scalar + dimensioned quantity

// Get a plain number out (no implicit unit -> double, except dimensionless)
double v = a.value();      // value in the quantity's units
double r = a.raw();        // stored value (differs from value() for percent/ppm)
double t = a.to<double>(); // explicit cast to a representation
int    i = a.to<int>();    // explicit, truncates toward zero

// Compare (same dimension only)
bool lt = (1.0_m < 2.0_ft);
// bool bad = (1.0_m < 1.0_kg);   // ERROR: incomparable dimensions

// Math — unqualified, found by ADL
meters h = sqrt(pow<2>(3.0_m) + pow<2>(4.0_m));   // 5 m
auto   s = sin(90.0_deg);          // trig needs an angle
// auto bad = sin(1.0_m);          // ERROR: sin needs an angle, not a length

// Name, print
std::cout << 5.0_m;                 // "5 m"
const char* n = (5.0_m).name();     // "meters"

// Dimensionless <-> arithmetic is implicit
dimensionless<double> ratio = 0.25; // double -> dimensionless
double back = ratio;                // dimensionless -> double
double frac = (50.0_pct).value();   // 0.5   (percent: .value() is the fraction)
double pts  = (50.0_pct).raw();     // 50    (.raw() is the point count)

// Constrain your own templates with the concepts
template <units::UnitType U> U twice(U x) { return x + x; }

// Define a unit in one line
namespace units {
    UNIT_ADD(length, smoots, smoot, conversion_factor<std::ratio<17018, 10000>, meters<>>)
}
auto bridge = 364.4_smoot;          // now a usable length, with its own literal
```

---

## Supported units

Every built-in unit, by dimension — **47 dimensions**, ~200 named units before metric prefixes. A unit
marked **yes** under Prefixes also provides every SI metric prefix from femto to peta (`_km`, `_mm`, …).
For a name shared across dimensions (e.g. `pounds`), qualify it: `units::mass::pounds` vs
`units::force::pounds`. This table is generated from the headers by `docs/reference/gen_reference.py`.

<!-- BEGIN generated: supported-units -->
### acceleration

| Unit | Literal | Prefixes |
|------|---------|----------|
| `meters_per_second_squared` | `_mps2` |  |
| `feet_per_second_squared` | `_fps2` |  |
| `standard_gravity` | `_SG` |  |
| `gals` | `_Gal` |  |

### angle

| Unit | Literal | Prefixes |
|------|---------|----------|
| `radians` | `_rad` | yes |
| `degrees` | `_deg` |  |
| `arcminutes` | `_arcmin` |  |
| `arcseconds` | `_arcsec` |  |
| `milliarcseconds` | `_mas` |  |
| `turns` | `_tr` |  |
| `gradians` | `_gon` |  |

### angular velocity

| Unit | Literal | Prefixes |
|------|---------|----------|
| `radians_per_second` | `_rad_per_s` |  |
| `degrees_per_second` | `_deg_per_s` |  |
| `revolutions_per_minute` | `_rpm` |  |
| `revolutions_per_second` | `_rps` |  |
| `milliarcseconds_per_year` | `_mas_per_yr` |  |

### area

| Unit | Literal | Prefixes |
|------|---------|----------|
| `square_meters` | `_m2` |  |
| `square_feet` | `_ft2` |  |
| `square_inches` | `_in2` |  |
| `square_miles` | `_mi2` |  |
| `square_kilometers` | `_km2` |  |
| `hectares` | `_ha` |  |
| `acres` | `_acre` |  |

### capacitance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `farads` | `_F` | yes |

### charge

| Unit | Literal | Prefixes |
|------|---------|----------|
| `coulombs` | `_C` | yes |
| `ampere_hours` | `_Ah` | yes |

### concentration

| Unit | Literal | Prefixes |
|------|---------|----------|
| `parts_per_million` | `_ppm` |  |
| `parts_per_billion` | `_ppb` |  |
| `parts_per_trillion` | `_ppt` |  |
| `percent` | `_pct` |  |

### conductance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `siemens` | `_S` | yes |

### current

| Unit | Literal | Prefixes |
|------|---------|----------|
| `amperes` | `_A` | yes |

### data

| Unit | Literal | Prefixes |
|------|---------|----------|
| `bytes` | `_B` |  |
| `exabytes` | `_EB` |  |
| `bits` | `_b` |  |
| `exabits` | `_Eb` |  |

### data transfer rate

| Unit | Literal | Prefixes |
|------|---------|----------|
| `bytes_per_second` | `_Bps` |  |
| `exabytes_per_second` | `_EBps` |  |
| `bits_per_second` | `_bps` |  |
| `exabits_per_second` | `_Ebps` |  |

### density

| Unit | Literal | Prefixes |
|------|---------|----------|
| `kilograms_per_cubic_meter` | `_kg_per_m3` |  |
| `grams_per_milliliter` | `_g_per_mL` |  |
| `kilograms_per_liter` | `_kg_per_L` |  |
| `ounces_per_cubic_foot` | `_oz_per_ft3` |  |
| `ounces_per_cubic_inch` | `_oz_per_in3` |  |
| `ounces_per_gallon` | `_oz_per_gal` |  |
| `pounds_per_cubic_foot` | `_lb_per_ft3` |  |
| `pounds_per_cubic_inch` | `_lb_per_in3` |  |
| `pounds_per_gallon` | `_lb_per_gal` |  |
| `slugs_per_cubic_foot` | `_slug_per_ft3` |  |

### energy

| Unit | Literal | Prefixes |
|------|---------|----------|
| `joules` | `_J` | yes |
| `calories` | `_cal` | yes |
| `kilowatt_hours` | `_kWh` |  |
| `watt_hours` | `_Wh` |  |
| `british_thermal_units` | `_BTU` |  |
| `british_thermal_units_iso` | `_BTU_iso` |  |
| `british_thermal_units_59` | `_BTU59` |  |
| `therms` | `_thm` |  |
| `foot_pounds` | `_ftlbf` |  |

### energy density

| Unit | Literal | Prefixes |
|------|---------|----------|
| `joules_per_meter_cubed` | `_J_per_m3` | yes |

### force

| Unit | Literal | Prefixes |
|------|---------|----------|
| `newtons` | `_N` | yes |
| `pounds` | `_lbf` |  |
| `dynes` | `_dyn` |  |
| `kiloponds` | `_kp` |  |
| `poundals` | `_pdl` |  |

### frequency

| Unit | Literal | Prefixes |
|------|---------|----------|
| `hertz` | `_Hz` | yes |

### illuminance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `lux` | `_lx` | yes |
| `footcandles` | `_fc` |  |
| `lumens_per_square_inch` | `_lm_per_in2` |  |
| `phots` | `_ph` |  |

### impedance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `ohms` | `_Ohm` | yes |

### inductance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `henries` | `_H` | yes |

### irradiance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_meter_squared` | `_W_per_m2` | yes |

### jerk

| Unit | Literal | Prefixes |
|------|---------|----------|
| `meters_per_second_cubed` | `_mps3` | yes |
| `feet_per_second_cubed` | `_fps3` |  |

### length

| Unit | Literal | Prefixes |
|------|---------|----------|
| `meters` | `_m` | yes |
| `feet` | `_ft` |  |
| `inches` | `_in` |  |
| `mils` | `_mil` |  |
| `miles` | `_mi` |  |
| `nautical_miles` | `_nmi` |  |
| `astronomical_units` | `_au` |  |
| `lightyears` | `_ly` |  |
| `parsecs` | `_pc` |  |
| `angstroms` | `_angstrom` |  |
| `cubits` | `_cbt` |  |
| `fathoms` | `_ftm` |  |
| `chains` | `_ch` |  |
| `furlongs` | `_fur` |  |
| `hands` | `_hand` |  |
| `leagues` | `_lea` |  |
| `nautical_leagues` | `_nl` |  |
| `yards` | `_yd` |  |

### luminance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `candelas_per_square_meter` | `_cd_per_m2` | yes |
| `stilbs` | `_sb` |  |
| `apostilbs` | `_asb` |  |
| `brils` | `_bril` |  |
| `skots` | `_sk` |  |
| `lamberts` | `_la` |  |
| `millilamberts` | `_mla` |  |
| `foot_lamberts` | `_ftL` |  |

### luminous flux

| Unit | Literal | Prefixes |
|------|---------|----------|
| `lumens` | `_lm` | yes |

### luminous intensity

| Unit | Literal | Prefixes |
|------|---------|----------|
| `candelas` | `_cd` | yes |

### magnetic field strength

| Unit | Literal | Prefixes |
|------|---------|----------|
| `teslas` | `_Te` | yes |
| `gauss` | `_G` |  |

### magnetic flux

| Unit | Literal | Prefixes |
|------|---------|----------|
| `webers` | `_Wb` | yes |
| `maxwells` | `_Mx` |  |

### mass

| Unit | Literal | Prefixes |
|------|---------|----------|
| `grams` | `_g` | yes |
| `tonnes` | `_t` |  |
| `pounds` | `_lb` |  |
| `long_tons` | `_ln_conversion_factor` |  |
| `short_tons` | `_sh_conversion_factor` |  |
| `stone` | `_st` |  |
| `ounces` | `_oz` |  |
| `carats` | `_ct` |  |
| `slugs` | `_slug` |  |

### power

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts` | `_W` | yes |
| `horsepower` | `_hp` |  |

### pressure

| Unit | Literal | Prefixes |
|------|---------|----------|
| `pascals` | `_Pa` | yes |
| `bars` | `_bar` |  |
| `millibars` | `_mbar` |  |
| `atmospheres` | `_atm` |  |
| `pounds_per_square_inch` | `_psi` |  |
| `torrs` | `_torr` |  |
| `millimeters_of_mercury` | `_mmHg` |  |
| `inches_of_mercury` | `_inHg` |  |

### radiance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_steradian_per_meter_squared` | `_W_per_srm2` | yes |

### radiant intensity

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_steradian` | `_W_per_sr` | yes |

### radiation

| Unit | Literal | Prefixes |
|------|---------|----------|
| `becquerels` | `_Bq` | yes |
| `grays` | `_Gy` | yes |
| `sieverts` | `_Sv` | yes |
| `curies` | `_Ci` |  |
| `rutherfords` | `_rd` |  |
| `radiation_absorbed_dose` | `_rads` |  |

### solid angle

| Unit | Literal | Prefixes |
|------|---------|----------|
| `steradians` | `_sr` | yes |
| `degrees_squared` | `_deg2` |  |
| `spats` | `_sp` |  |

### spectral flux

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_meter` | `_W_per_m` | yes |

### spectral intensity

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_steradian_per_meter` | `_W_per_srm` | yes |

### spectral irradiance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_meter_cubed` | `_W_per_m3` | yes |

### spectral radiance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `watts_per_steradian_per_meter_cubed` | `_W_per_srm3` | yes |

### substance

| Unit | Literal | Prefixes |
|------|---------|----------|
| `mols` | `_mol` | yes |

### substance concentration

| Unit | Literal | Prefixes |
|------|---------|----------|
| `molars` | `_M` | yes |

### substance mass

| Unit | Literal | Prefixes |
|------|---------|----------|
| `grams_per_mole` | `_g_per_mol` | yes |

### temperature

| Unit | Literal | Prefixes |
|------|---------|----------|
| `kelvin` | `_K` |  |
| `celsius` | `_degC` |  |
| `fahrenheit` | `_degF` |  |
| `reaumur` | `_Re` |  |
| `rankine` | `_Ra` |  |

### time

| Unit | Literal | Prefixes |
|------|---------|----------|
| `seconds` | `_s` | yes |
| `minutes` | `_min` |  |
| `hours` | `_hr` |  |
| `days` | `_d` |  |
| `weeks` | `_wk` |  |
| `years` | `_yr` |  |
| `julian_years` | `_a_j` |  |
| `gregorian_years` | `_a_g` |  |

### torque

| Unit | Literal | Prefixes |
|------|---------|----------|
| `newton_meters` | `_Nm` |  |
| `foot_pounds` | `_ftlb` |  |
| `foot_poundals` | `_ftpdl` |  |
| `inch_pounds` | `_inlb` |  |
| `meter_kilograms` | `_mkgf` |  |

### velocity

| Unit | Literal | Prefixes |
|------|---------|----------|
| `meters_per_second` | `_mps` |  |
| `feet_per_second` | `_fps` |  |
| `miles_per_hour` | `_mph` |  |
| `kilometers_per_hour` | `_kph` |  |
| `knots` | `_kts` |  |

### voltage

| Unit | Literal | Prefixes |
|------|---------|----------|
| `volts` | `_V` | yes |
| `statvolts` | `_statV` |  |
| `abvolts` | `_abV` |  |

### volume

| Unit | Literal | Prefixes |
|------|---------|----------|
| `cubic_meters` | `_m3` |  |
| `cubic_millimeters` | `_mm3` |  |
| `cubic_kilometers` | `_km3` |  |
| `liters` | `_L` | yes |
| `cubic_inches` | `_in3` |  |
| `cubic_feet` | `_ft3` |  |
| `cubic_yards` | `_yd3` |  |
| `cubic_miles` | `_mi3` |  |
| `gallons` | `_gal` |  |
| `quarts` | `_qt` |  |
| `pints` | `_pt` |  |
| `cups` | `_c` |  |
| `fluid_ounces` | `_fl_oz` |  |
| `barrels` | `_bl` |  |
| `bushels` | `_bu` |  |
| `cords` | `_cord` |  |
| `cubic_fathoms` | `_fm3` |  |
| `tablespoons` | `_tbsp` |  |
| `teaspoons` | `_tsp` |  |
| `pinches` | `_pinch` |  |
| `dashes` | `_dash` |  |
| `drops` | `_drop` |  |
| `fifths` | `_fifth` |  |
| `drams` | `_dr` |  |
| `gills` | `_gi` |  |
| `pecks` | `_pk` |  |
| `sacks` | `_sck` |  |
| `shots` | `_shts` |  |
| `strikes` | `_strk` |  |
<!-- END generated: supported-units -->

---

## Physical constants

Provided in `units::constants` as typed quantities (each carries its dimension, so it participates in
dimensional analysis). Values are the 2018 CODATA recommended values.

<!-- BEGIN generated: constants -->
| Symbol | Constant | Value |
|--------|----------|-------|
| `pi` | Ratio of a circle's circumference to its diameter | 1 |
| `c` | Speed of light in vacuum | 299792458.0 |
| `G` | Newtonian constant of gravitation | 6.67430e-11 |
| `h` | Planck constant | 6.62607015e-34 |
| `h_bar` | Reduced Planck constant | 1.054571817e-34 |
| `mu0` | vacuum permeability | 1.25663706212e-6 |
| `epsilon0` | vacuum permittivity | 8.8541878128e-12 |
| `Z0` | characteristic impedance of vacuum | 376.730313668 |
| `k_e` | Coulomb's constant | 8.9875517923e9 |
| `e` | elementary charge | 1.602176634e-19 |
| `m_e` | electron mass | 9.1093837015e-31 |
| `m_p` | proton mass | 1.67262192369e-27 |
| `mu_B` | Bohr Magneton | 9.2740100783e-24 |
| `N_A` | Avogadro's Number | 6.02214076e23 |
| `R` | Gas constant | 8.314462618 |
| `k_B` | Boltzmann constant | 1.380649e-23 |
| `F` | Faraday constant | 96485.33212 |
| `sigma` | Stefan-Boltzmann constant | 5.670374419e-8 |
<!-- END generated: constants -->

---

## More capabilities

Beyond the catalog: unit-aware `<cmath>` (found by ADL), `std::chrono::duration` interop, `std::hash`
and `std::numeric_limits` specializations, NaN/infinity support, optional
[nlohmann/json](docs/how-to/json-serialization.md) serialization, a concept vocabulary (`UnitType`,
`ConversionFactorType`, …) for constraining your own templates, non-linear (decibel) scales, and affine
temperature. Each has a how-to or reference page under [docs/](docs/).

---

## Documentation

The manual is under **[docs/](docs/)** (hub: [docs/README.md](docs/README.md)). The generated API
reference is published at <https://nholthaus.github.io/units/>.

### Learn

- [Getting started](docs/learn/getting-started.md)
- [First quantities](docs/learn/first-quantities.md)
- [Unit conversions](docs/learn/unit-conversions.md)

### Explain

- [Why units](docs/explain/why-units.md)
- [Dimensional analysis](docs/explain/dimensional-analysis.md)
- [Type safety](docs/explain/type-safety.md)
- [CTAD and ADL](docs/explain/ctad-and-adl-for-humans.md)
- [Efficiency](docs/explain/efficiency.md)
- [Scales](docs/explain/scales.md)
- [Affine temperature](docs/explain/affine-temperature.md)
- [Namespaces](docs/explain/namespaces.md)
- [Named-type internals](docs/explain/internals-named-types.md)

### How-to

- [Defining new units](docs/how-to/defining-new-units.md)
- [Math functions](docs/how-to/math-functions.md)
- [chrono interop](docs/how-to/chrono-interop.md)
- [JSON serialization](docs/how-to/json-serialization.md)
- [Disabling iostream](docs/how-to/disabling-iostream.md)
- [Subset headers for compile time](docs/how-to/subset-headers-compile-time.md)
- [Visual Studio visualizer](docs/how-to/natvis.md)
- [CMake integration](docs/how-to/cmake-integration.md)

### Reference

- [Cheat sheet](docs/reference/cheat-sheet.md)
- [Supported units](docs/reference/supported-units.md)
- [Constants](docs/reference/constants.md)
- [Literals](docs/reference/literals.md)
- [Type traits](docs/reference/type-traits.md)
- [Concepts](docs/reference/concepts.md)
- [Configuration macros](docs/reference/configuration.md)

### Meta

- [FAQ](docs/meta/faq.md)
- [Migrating from 2.x](docs/meta/migrate-v2-to-v3.md)
- [Changelog](CHANGELOG.md)

---

## License

`units` is distributed under the [MIT License](LICENSE). Copyright © Nic Holthaus.
