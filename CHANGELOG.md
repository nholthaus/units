# Changelog

All notable changes to this project are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/), and the project adheres to semantic versioning.

## [3.4.1] - 2026-08-14

A documentation and maintenance release. 3.x becomes the project's default branch.

### Added
- A complete documentation set: a rewritten README (with an inlined cheat sheet, the full unit catalog,
  and the physical-constants table) and an in-repo `docs/` manual (learn / explain / how-to / reference /
  meta). The Doxygen reference is published to <https://nholthaus.github.io/units/> from CI, and every
  documented code snippet is compiled as part of the test suite.

### Fixed
- `units::modf()` applied a scaled dimensionless unit's scale twice to the fractional part; for `percent`,
  `modf(202.5%)` returned a fractional part of `0.00025` instead of `0.025`. The fractional part is now
  returned as a dimensionless value, so the scale is applied once. (#312)
- `minutes` had no registered name or abbreviation (it printed as its base unit and could return a null
  `name()`/`abbreviation()`); it now reports `"minutes"` / `"min"` and prints as `1.5 min`.

## [3.4.0] - 2026-08-14

The readability release: compiler diagnostics now name the friendly unit type.

### Added
- **Readable diagnostics.** Named units are now class templates deriving from `unit<...>`, so a compiler
  error, a debugger, and `std::common_type` all report `meters<double>` instead of the underlying
  `unit<conversion_factor<...>>` template — for operands *and* computed results. Bare-name usage
  (`meters`, `meters<>`, `meters<double>`) and CTAD (`meters(5.0)`) are unchanged; the types remain
  trivially copyable.
- **Visual Studio debugger visualizer** (`natvis/units.natvis`), attached to the CMake interface target
  and installed with the package, so a quantity shows as `5 m` in the debugger under MSVC.
- **Error-message test harness** (`test/errorMessages/`): a corpus of deliberately ill-formed cases,
  compiled in CI, that asserts each mistake is rejected and that the diagnostic names the friendly type.

### Fixed
- **Include-order regression (#357).** An expression that reduces to a not-yet-included dimension now
  compiles. The strong-type registration was reworked from an explicit specialization into an ADL
  customization point, so a later-included dimension header can never be "declared after instantiation."
- Standard-library extensions (`std::hash`, `std::numeric_limits`, the classification functions) that had
  continued to use the removed `operator()` now use `.raw()`.

## [3.3.0] - 2026-01-07

### Fixed
- Concentration/ratio-dimensionless (`percent`, `ppm`, …) arithmetic semantics: `.value()` returns the
  normalized fraction and `.raw()` the point count, and scalar/quantity operations with a
  ratio-dimensionless operand compute consistently.
- `floor` and `ceil` are now `constexpr`.

## [3.2.0] - 2025-12-18

### Added
- A `unit` may be used as a non-type template parameter (NTTP).

### Changed
- `operator<<` implementation simplified using a fold expression.

## [3.1.2] - 2025-09-26

### Added
- CMake package export cleanup and CPack configuration for tarball, Debian, and RPM artifacts; Debian
  packaging rules for a PPA.

### Changed
- Updated the bundled GoogleTest to 1.17.0 and raised the supported CMake range.

## [3.1.0] - 2025-01-16

The C++23 line. This is a major revision; see the
[migration guide](docs/meta/migrate-v2-to-v3.md) for the details.

### Changed
- **Requires C++23** (the 2.x series targeted C++14).
- **Named units use the plural class-template spelling** (`meters`, `meters<double>`); the singular `_t`
  aliases (`meter_t`) are removed. CTAD makes the angle brackets optional.
- **Value extraction is `.value()` / `.raw()` / `.to<T>()`;** `operator()` is removed.
- **Math functions moved from `units::math` into `units`** and are found by argument-dependent lookup —
  call them unqualified (`sqrt(x)`); the `units::math` namespace is removed.
- **Dimension namespaces are inline:** `units::meters` resolves (the `units::length::meters` form still
  works for disambiguation).
- Core machinery renamed: `unit_t` → `unit`, `base_unit`/category → `dimension`, the `unit` tag →
  `conversion_factor`, `cpow` → `pow`.
- SFINAE constraints replaced with C++20 concepts, exposed as a public vocabulary (`UnitType`,
  `ConversionFactorType`, `same_dimension`, …).

### Added
- Unit constants: `1.0 * units::m` alongside the `1.0_m` literals.
- `std::chrono::duration` interoperability, `std::hash` and `std::numeric_limits` specializations,
  NaN/infinity support, `constexpr` `<cmath>` wrappers on supporting compilers, optional nlohmann/json
  serialization, and a large set of additional units (including a radiometry suite).

### Removed
- `unit_value_t` — use a `constexpr` quantity value instead.
- The `units::math` namespace and the `_t` singular type aliases (see Changed).

[3.4.0]: https://github.com/nholthaus/units/releases/tag/v3.4.0
[3.3.0]: https://github.com/nholthaus/units/releases/tag/v3.3.0
[3.2.0]: https://github.com/nholthaus/units/releases/tag/v3.2.0
[3.1.2]: https://github.com/nholthaus/units/releases/tag/v3.1.2
[3.1.0]: https://github.com/nholthaus/units/releases/tag/v3.1.0-beta
