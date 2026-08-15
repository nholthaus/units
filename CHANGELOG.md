# Changelog

All notable changes to this project are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/), and the project adheres to semantic versioning.

## [3.5.1] - 2026-08-15

### Fixed
- The installed CMake package now reports version 3.5.1 instead of the stale 3.4.0, so versioned
  `find_package(units 3.5.1 EXACT)` requests work.
- The Debian package now stages files under the correct `libunits-dev` package directory instead of producing
  a package without the library headers and CMake metadata.
- Debugger helpers now install under the standard shared-data location instead of directly under `/usr`.
- Installing a build configured with tests no longer installs the bundled GoogleTest dependency.

## [3.5.0] - 2026-08-15

New capabilities: first-class Eigen interoperability, self-describing serialization, and an LLDB debugger
visualizer.

### Added
- Self-describing binary serialization (`units/serialization.h`, an opt-in header not pulled in by `units.h`):
  `serialize(q)` encodes a quantity's dimension and value to a compact byte stream, and `deserialize(bytes)`
  recovers it with no prior agreement on the type, returning an erased `any_unit` that collapses to a concrete
  quantity via `to<Unit>()` (checked), `assign_to(out)` (mismatch-tolerant, assigns into an existing variable
  and returns whether the dimension fit), `try_to<Unit>()`/`unit_cast<Unit>()` (throwing), or `visit()` (the
  decoded dimension's canonical unit). `to_string()` renders the SI-base magnitude in the decoded dimension's
  named canonical unit when the library knows the dimension (`100 m`, `9.81 m s^-2`), and `to_string_raw()`
  gives the always-available name-free form (the hashed dimension signature). Base dimensions are keyed by a
  name hash, so any dimension round-trips — including one defined with `make_dimension<>` — with no fixed
  ceiling and no reflection.
- Optional, dependency-free interoperability with the [Eigen](https://eigen.tuxfamily.org) linear-algebra
  library (`units/eigen.h`, pulled in by `units.h`): a unit is usable as an Eigen matrix scalar, so vectors and
  matrices can hold dimensioned quantities with the dimensions checked at compile time. Same-dimension
  operations (construction, add/subtract, scaling, `sum()`, blocks, `Map`, `cast`) work directly on Eigen
  expressions; the dimension-changing operations are provided as helpers with the correct result type —
  `unit_dot`, `unit_squared_norm`, `unit_norm`, `unit_normalized`, `unit_cross`, and `unit_transform`. The
  support activates only when `<Eigen/Core>` is present (guarded by `__has_include`); `units` gains no
  dependency on Eigen. (#90)
- An LLDB debugger formatter (`natvis/units_lldb.py`) so a quantity shows as its value and abbreviation
  (`5 m`) in LLDB and its front-ends (CLion, Xcode, CodeLLDB), mirroring the MSVC natvis. (#279)

### Fixed
- `to_string(unit)` on a unit with no registered abbreviation (rendered by its dimension form, e.g. the
  canonical unit of `acceleration`) failed to compile and, once fixed, is now single-spaced to match
  `operator<<` exactly (`9.81 m s^-2`, not `9.81  m s^-2`).

## [3.4.4] - 2026-08-15

Backlog cleanup: a correctness fix, new units, clearer naming, and a lossy-scale warning.

### Added
- A `dynamic_viscosity` dimension (`pressure * time`) with `pascal_seconds`, `poise`, and `centipoise`, and a
  `kinematic_viscosity` dimension (`area / time`) with `square_meters_per_second`, `stokes`, and
  `centistokes`. (#205)
- The `biots` unit of current, an alias of `abamperes` (the CGS-EMU name). (#205)
- The how-to guide for defining new units now covers naming a derived unit type inline with `decltype`
  (no macro) and building a custom, deliberately-incompatible dimension from a base-dimension tag via
  `make_dimension`. (#133, #281)

### Changed
- The hyperbolic functions `cosh`/`sinh`/`tanh` now take a dimensionless argument and `acosh`/`asinh`/`atanh`
  now return a dimensionless value, matching the mathematical definitions. Passing an angle or other
  dimensioned quantity is rejected at compile time. This is a breaking change for code that relied on the
  previous angle-based signatures. (#285)
- Torque's conventionally-named unit is the pound-foot: `units::torque::pound_feet` (abbreviation `lbf_ft`).
  `units::torque::foot_pounds` is now a deprecated alias of it; `units::energy::foot_pounds` (the energy unit)
  is unchanged. (#311)
- The compound-assignment operators (`*=`, `/=`) keep the right-hand side's own arithmetic type. Scaling an
  integer-backed unit by a floating-point factor is lossy and now surfaces the compiler's float-to-integer
  conversion warning (naming the friendly underlying type) instead of truncating silently. It remains a
  warning, not a hard error. (#257)

## [3.4.3] - 2026-08-15

Issue-triage follow-up: precision, ergonomics, and a new dimension.

### Added
- A `volume_flow_rate` dimension (`volume / time`) with `cubic_meters_per_second`, `cubic_meters_per_hour`,
  `liters_per_second`, `liters_per_minute`, `gallons_per_minute`, `gallons_per_hour`,
  `cubic_feet_per_second`, and `cubic_feet_per_minute`. Because the dimension is derived algebraically, a
  volume divided by a time is a `volume_flow_rate`. (#112)
- A named-unit `to<>()` accessor: `q.to<meters>()` converts to a named unit of the same dimension, mirroring
  `q.convert<meters>()`; `q.to<double>()` (underlying-type extraction) is unchanged. (#303)
- A `_dB` literal for the dimensionless decibel, and its registered name/abbreviation. (#334, #344)

### Fixed
- The `slug` used a rounded conversion ratio (about 2.5 ppb low); it now uses the exact rational. (#289)
- A decibel-scale unit now requires a floating-point underlying type: an integral type cannot represent a
  logarithmic value (`3 dB` stored as `0`, large values overflowed). The integer decibel literals are
  removed and the guard is enforced at compile time. (#334, #344)
- The named `convert<>()` overload is now callable on a `const` unit.

### Changed
- `data::bytes`/`bits` are spelled out with only the meaningful large decimal (kilo and up) and binary
  prefixes; the sub-unit prefixes (deci/centi/milli/...) that produced fractional-byte units are dropped.
  This also frees the `_dB` literal, which `decibytes` had claimed.

## [3.4.2] - 2026-08-14

A units-and-correctness release: a broad set of new units, and a conversion fix.

### Added
- Over fifty new units across many dimensions — imperial and US-customary, ancient, and esoteric — each
  defined by an exact rational against its most-canonical parent and verified against its authoritative
  value. Highlights: length `rods`/`links`/`barleycorns`/`nails`/`spans`/`picas`/`points`; velocity
  `feet_per_minute`/`meters_per_minute`/`inches_per_second`/`kilometers_per_second`; area
  `roods`/`square_rods`; angle `angular_mils`/`compass_points`; time
  `fortnights`/`decades`/`centuries`/`millennia`; `nibbles`; radiation `rem`; substance `pound_moles`;
  mass `grains`/`avoirdupois_drams`/`pennyweights`/`troy_ounces`/`troy_pounds`/`hundredweights`; force
  `kips`/`ounces_force`/`grams_force`/`short_tons_force`/`long_tons_force`/`sthenes`; pressure
  `technical_atmospheres`/`pounds_per_square_foot`/`kips_per_square_inch`/`baryes`/`piezes`/water columns;
  energy `ergs`/`calories_it`/`tons_of_tnt`; power `metric_horsepower`/`electrical_horsepower`/
  `tons_of_refrigeration`; and the CGS `ab-`/`stat-` charge and current pairs.

### Fixed
- The `statvolt` conversion was inverted (`1 statvolt` read as `0.00333564 V` instead of `299.792458 V`).
  Corrected to `c / 1e6` volts. `abvolts` was already correct.

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

[3.5.1]: https://github.com/nholthaus/units/releases/tag/v3.5.1
[3.5.0]: https://github.com/nholthaus/units/releases/tag/v3.5.0
[3.4.4]: https://github.com/nholthaus/units/releases/tag/v3.4.4
[3.4.3]: https://github.com/nholthaus/units/releases/tag/v3.4.3
[3.4.2]: https://github.com/nholthaus/units/releases/tag/v3.4.2
[3.4.1]: https://github.com/nholthaus/units/releases/tag/v3.4.1
[3.4.0]: https://github.com/nholthaus/units/releases/tag/v3.4.0
[3.3.0]: https://github.com/nholthaus/units/releases/tag/v3.3.0
[3.2.0]: https://github.com/nholthaus/units/releases/tag/v3.2.0
[3.1.2]: https://github.com/nholthaus/units/releases/tag/v3.1.2
[3.1.0]: https://github.com/nholthaus/units/releases/tag/v3.1.0-beta
