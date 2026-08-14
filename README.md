# units

A compile-time, header-only, dimensional-analysis and unit-conversion library for **C++23**, with no
dependencies.

![Windows build](https://github.com/nholthaus/units/actions/workflows/msvc-2022.yaml/badge.svg)
![GCC build](https://github.com/nholthaus/units/actions/workflows/gcc-13.yaml/badge.svg)
![Clang build](https://github.com/nholthaus/units/actions/workflows/clang-19.yaml/badge.svg)
![license](https://img.shields.io/badge/license-MIT-orange.svg)
![copyright](https://img.shields.io/badge/%C2%A9-Nic_Holthaus-orange.svg)
![standard](https://img.shields.io/badge/std-c%2B%2B23-blue.svg)

`units` lets you write physical quantities as types. A length is a `meters`, not a `double` with a
comment. Conversions between compatible units happen implicitly and are resolved entirely at compile
time, so they cost nothing at run time; expressions that are dimensionally wrong do not compile.

```cpp
#include <units/length.h>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

    meters distance = 5.0_m;    // a length, deduced as meters<double>
    feet   in_feet  = distance; // implicit, lossless unit conversion

    std::cout << distance << " == " << in_feet << '\n';   // prints: 5 m == 16.4042 ft
}
```

Every snippet in this README and in the [documentation](docs/) is compiled and run as part of the test
suite — see [`examples/`](examples/). Copy any of them; they build under C++23 as-is.

---

## Design goals

- **Intuitive syntax.** Quantities read like the physics: `meters`, `60_mi / 1_hr`, `sqrt(area)`. You
  work in the unit domain and let the library convert.
- **Zero run-time cost.** Conversions are `constexpr` ratios; a conversion between equivalent
  representations compiles to no machine code at all. A unit is a trivially-copyable value the size of
  its underlying type.
- **Dimensional safety.** Adding a length to a time, or assigning an area to a length, is a compile
  error — not a run-time surprise. The dimensional analysis is checked by the type system.
- **Readable diagnostics.** When something *is* wrong, the compiler names the friendly type
  (`meters<double>`), not an unreadable `conversion_factor<...>` template. See
  [When it doesn't compile](#when-it-doesnt-compile-thats-the-point).
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

Include the umbrella header `<units.h>` for everything, or a single dimension header such as
`<units/length.h>` for a lighter build, and bring in the literal operators:

```cpp
#include <units.h>
using namespace units;
using namespace units::literals;
```

There are four equivalent ways to make a quantity — pick whichever reads best:

```cpp
meters a(5.0);          // construction (CTAD deduces meters<double>)
meters b = 5.0_m;       // a unit literal
auto   c = 5.0 * m;     // a scalar times a unit constant
meters d{5.0};          // braced construction
```

Arithmetic produces the correct dimension automatically, and you can name the result to have the
compiler verify it:

```cpp
square_meters     area  = 15.0_m * 5.0_m;       // m * m -> area
meters_per_second speed = 60.0_mi / 1.0_hr;      // a more involved conversion, still implicit
meters            side  = sqrt(area / 3.0);      // <cmath> functions are unit-aware (found by ADL)
```

New to how `meters a(5.0)` deduces its type, or why `sqrt` needs no `units::` prefix? Read
**[CTAD and ADL, for people who don't like templates](docs/explain/ctad-and-adl-for-humans.md)** — it
explains, in plain terms, what you type and what you get.

> **Note — write the decimal point for fractional values.** A literal's type follows what you write:
> `5.0_m` is `meters<double>`, but `5_m` is `meters<int>`. Integer-backed quantities do integer
> arithmetic, so `1_m / 2_m` is `0`, whereas `1.0_m / 2.0_m` is `0.5`. Use a decimal point (or an
> explicit `meters<double>`) when you want fractional results.

The full walkthrough is in [docs/learn/getting-started.md](docs/learn/getting-started.md).

---

## When it doesn't compile, that's the point

The reason to spend types on your quantities is that the compiler catches the mistakes a bare `double`
would let through. In 3.x those diagnostics name the *friendly* type, so they are actually readable.

Adding incompatible dimensions is rejected, and the message says exactly what you tried to add
(captured verbatim from GCC 13):

```text
readable_add_incompatible.cpp:9:18: error: no match for ‘operator+’ (operand types are ‘units::length::meters<double>’ and ‘units::time::seconds<double>’)
    9 | auto bad = 1.0_m + 1.0_s;
      |            ~~~~~ ^ ~~~~~
      |            |       |
      |            |       units::time::seconds<double>
      |            units::length::meters<double>
```

So is claiming a product is the wrong dimension — `m * m` is an area, not a length. GCC surfaces the
result through an internal alias, but names the friendly type right beside it (`{aka …}`):

```text
readable_wrong_result_type.cpp:10:41: error: conversion from ‘units::detail::rewrap_to_named_t<units::unit<units::area::square_meters_, double, units::linear_scale> >’ {aka ‘units::area::square_meters<double>’} to non-scalar type ‘units::length::meters<double>’ requested
   10 | units::length::meters<double> a = 1.0_m * 1.0_m;
      |                                   ~~~~~~^~~~~~~
```

The complete set of mistakes the library is designed to reject — and the real diagnostic each produces
on GCC, Clang, and MSVC — is in [docs/explain/type-safety.md](docs/explain/type-safety.md). Those
diagnostics are captured directly from the compilers by the test harness, so they never drift from what
you will actually see.

---

## No run-time cost

Unit conversions are computed at compile time. A recursively-defined conversion (years → weeks, defined
through days, hours, minutes, and seconds) collapses to a single multiply/divide; a conversion between
equivalent representations generates no code at all. Quantities are trivially copyable and occupy
exactly the space of their underlying type — there is no wrapper overhead.

```cpp
static_assert(1.0_km + 1.0_m == 1001.0_m);   // a fact, established at compile time
```

See [docs/explain/efficiency.md](docs/explain/efficiency.md) for the generated assembly and the details.

---

## Integration

`units` is header-only. Choose whichever fits your build.

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

## What's in the box

A large catalog of SI and non-SI units across **47 dimensions** — length, mass, time, angle, temperature,
energy, power, pressure, frequency, velocity, acceleration, force, charge, voltage, data and data-rate, a
full radiometry suite, and more — plus a set of physical constants (`c`, `G`, `h`, `N_A`, `k_B`, …). The
browsable catalog is in [docs/reference/supported-units.md](docs/reference/supported-units.md) and the
constants in [docs/reference/constants.md](docs/reference/constants.md).

Also provided: unit-aware `<cmath>` (found by ADL), `std::chrono::duration` interop, `std::hash` and
`std::numeric_limits` specializations, NaN/infinity support, optional
[nlohmann/json](docs/how-to/json-serialization.md) serialization, and a concept vocabulary
(`UnitType`, `ConversionFactorType`, …) for constraining your own templates.

---

## Documentation

The full manual lives in **[docs/](docs/)**. Start with:

- **[Getting started](docs/learn/getting-started.md)** — from `#include` to your first quantities.
- **[CTAD and ADL, for people who don't like templates](docs/explain/ctad-and-adl-for-humans.md)** — the
  two features that make the syntax pleasant, in plain language.
- **[Cheat sheet](docs/reference/cheat-sheet.md)** — the whole API on one page.
- **[Migrating from 2.x](docs/meta/migrate-v2-to-v3.md)** — what changed and how to update.
- **[Defining your own units](docs/how-to/defining-new-units.md)** — one line adds a unit.
- **[Supported units](docs/reference/supported-units.md)** and **[constants](docs/reference/constants.md)**.
- **[FAQ](docs/meta/faq.md)** and **[Changelog](CHANGELOG.md)**.

The generated API reference (classes, namespaces, the full unit list) is published at
<https://nholthaus.github.io/units/>.

---

## License

`units` is distributed under the [MIT License](LICENSE). Copyright © Nic Holthaus.
