# units documentation

The manual for the [units](https://github.com/nholthaus/units) library, grouped by what you are trying to do. Every code
snippet in these pages compiles under C++23; the runnable ones are drawn from [`examples/`](../examples/),
and the compiler diagnostics shown are captured verbatim from the compilers.

Start with [Getting started](learn/getting-started.md); the
[cheat sheet](reference/cheat-sheet.md) is the API on one page.

## Learn

- [Getting started](learn/getting-started.md)
- [First quantities](learn/first-quantities.md)
- [Unit conversions](learn/unit-conversions.md)

## Explain

- [Why units](explain/why-units.md)
- [Dimensional analysis](explain/dimensional-analysis.md)
- [Type safety](explain/type-safety.md)
- [CTAD and ADL](explain/ctad-and-adl-for-humans.md)
- [Efficiency](explain/efficiency.md)
- [Scales](explain/scales.md)
- [Affine temperature](explain/affine-temperature.md)
- [Namespaces](explain/namespaces.md)
- [Named-type internals](explain/internals-named-types.md)
- [Naming computed results consistently](explain/naming-computed-results.md)

## How-to

- [Defining new units](how-to/defining-new-units.md)
- [Math functions](how-to/math-functions.md)
- [chrono interop](how-to/chrono-interop.md)
- [Serialization](how-to/serialization.md)
- [JSON serialization](how-to/json-serialization.md)
- [Eigen interoperability](how-to/eigen.md)
- [Disabling iostream](how-to/disabling-iostream.md)
- [Subset headers for compile time](how-to/subset-headers-compile-time.md)
- [Debugger visualizers](how-to/natvis.md)
- [CMake integration](how-to/cmake-integration.md)

## Reference

- [Cheat sheet](reference/cheat-sheet.md)
- [Supported units](reference/supported-units.md)
- [Constants](reference/constants.md)
- [Literals](reference/literals.md)
- [Type traits](reference/type-traits.md)
- [Concepts](reference/concepts.md)
- [Configuration macros](reference/configuration.md)

## Meta

- [FAQ](meta/faq.md)
- [Migrating from 2.x](meta/migrate-v2-to-v3.md)
- [Changelog](https://github.com/nholthaus/units/blob/master/CHANGELOG.md)

The generated API reference (classes, namespaces, the full unit list) is published at
<https://nholthaus.github.io/units/>.
