# units documentation

The manual for the [units](https://github.com/nholthaus/units) library, grouped by what you are trying to do. Every code
snippet in these pages compiles under C++23; the runnable ones are drawn from [`examples/`](../examples/),
and the compiler diagnostics shown are captured verbatim from the compilers.

New here? Read [Getting started](learn/getting-started.md), then keep the
[cheat sheet](reference/cheat-sheet.md) open.

## Learn

Start-to-finish introductions.

- [Getting started](learn/getting-started.md) — from `#include` to your first quantities.
- [First quantities](learn/first-quantities.md) — constructing, converting, and printing.
- [Unit conversions](learn/unit-conversions.md) — how implicit conversion works and when it won't.

## Explain

The concepts and the reasoning behind them.

- [Why units](explain/why-units.md) — the bugs it prevents and the case for typed quantities.
- [Dimensional analysis](explain/dimensional-analysis.md) — how arithmetic tracks dimensions.
- [Type safety](explain/type-safety.md) — the mistakes the library rejects, with real diagnostics.
- [CTAD and ADL, for people who don't like templates](explain/ctad-and-adl-for-humans.md) — the two
  features behind the terse syntax, in plain language.
- [Efficiency](explain/efficiency.md) — why conversions cost nothing at run time.
- [Scales](explain/scales.md) — linear vs. decibel (non-linear) units.
- [Affine temperature](explain/affine-temperature.md) — why celsius and fahrenheit carry an offset.
- [Namespaces](explain/namespaces.md) — the `units`, `literals`, `constants`, and `traits` map.
- [Named-type internals](explain/internals-named-types.md) — *advanced:* how the readable-diagnostics
  machinery is built. Not required to use the library.

## How-to

Task-focused recipes.

- [Defining new units](how-to/defining-new-units.md) — add your own unit in one line.
- [Math functions](how-to/math-functions.md) — the unit-aware `<cmath>`.
- [chrono interop](how-to/chrono-interop.md) — converting to and from `std::chrono::duration`.
- [JSON serialization](how-to/json-serialization.md) — optional nlohmann/json support.
- [Disabling iostream](how-to/disabling-iostream.md) — for embedded builds.
- [Subset headers for compile time](how-to/subset-headers-compile-time.md) — include only what you use.
- [Visual Studio visualizer](how-to/natvis.md) — the natvis debugger view.
- [CMake integration](how-to/cmake-integration.md) — add_subdirectory, FetchContent, find_package.

## Reference

Look-up material.

- [Cheat sheet](reference/cheat-sheet.md) — the whole API on one page.
- [Supported units](reference/supported-units.md) — the full catalog, by dimension.
- [Constants](reference/constants.md) — the physical constants.
- [Literals](reference/literals.md) — the `_m`, `_s`, … operators and metric prefixes.
- [Type traits](reference/type-traits.md) — the `units::traits` catalog.
- [Concepts](reference/concepts.md) — `UnitType`, `ConversionFactorType`, … for your own templates.
- [Configuration macros](reference/configuration.md) — `UNIT_LIB_DEFAULT_TYPE` and the CMake options.

## Meta

- [FAQ](meta/faq.md) — common questions and their answers.
- [Migrating from 2.x](meta/migrate-v2-to-v3.md) — what changed and how to update.
- [Changelog](https://github.com/nholthaus/units/blob/master/CHANGELOG.md) — the release history.

The generated API reference (classes, namespaces, the full unit list) is published at
<https://nholthaus.github.io/units/>.
