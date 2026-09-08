# Changelog

All notable changes to this project are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/), and the project adheres to semantic versioning.

## [Unreleased]

Affine temperatures and decibel scales become first-class: the arithmetic real formulae need compiles, and the
operations that cannot mean anything say so in one sentence naming the remedy.

The rule is that an operation reads a quantity's number **in the scale that number is written in**, and is available
wherever that reading is well defined. `celsius(20) * 2.0` is 40 °C, `abs(celsius(-5.25))` is 5.25 °C, and a weighted sum
of readings works — which is what published temperature formulae are made of. Sixteen of the twenty-six collected in
`test/main.cpp::caseStudyPublishedTemperatureFormulae` scale a °C or °F reading directly, and Magnus's has no
absolute-scale form at all. Refused is an operation with no single reading: a bare number moved into a quantity, and a
transcendental function or a product of a **decibel** value, whose stored number is a logarithm and not the ratio it
denotes.

### Added

- **A reading moves by an amount, in every spelling.** `celsius(20) + kelvin(5)` and `kelvin(5) + celsius(20)` are both
  25 °C — a reading and an amount commute, and the answer is in the reading's unit. `reading + reading` reads its right
  operand as an amount on the same terms, so `0.7*Tnw + 0.2*Tg + 0.1*Ta` and mean radiant temperature's
  `(Tg - Ta) * (1 + 0.22*sqrt(v)) + Ta` compile and give the published answers. Two readings do not commute: the result
  takes the left operand's unit and reads the right one in that unit's degrees, so `celsius(20) + fahrenheit(9)` is
  25 °C while `fahrenheit(9) + celsius(20)` is 45 °F. Write the operand whose scale you want the answer in on the left.
- **`units::lerp(a, b, t)` and `units::midpoint(a, b)`** for any two same-dimension quantities on a linear or affine
  scale. For readings these are the datum-independent weighting: a weighted sum is scale-independent exactly when its
  weights total one, so `midpoint(celsius(20), celsius(30))` is 25 °C and the same two temperatures in kelvin give the
  same physical answer. A floating-point pair goes to `std::lerp`/`std::midpoint`, inheriting exactness at the
  endpoints, monotonicity and freedom from overflow, and an integral pair is halved in a double-width intermediate:
  `midpoint(meters<int>(INT_MIN), meters<int>(INT_MAX))` is −1 and `midpoint(meters<double>(inf), meters<double>(1))`
  is `inf`.
- **Two traits so generic code can ask rather than carry a unit list**: `units::traits::has_arbitrary_origin_v<U>` (an
  affine reading or a decibel level) and `units::traits::is_decibel_level_v<U>`. Both answer for any type, a non-unit
  included, and `has_arbitrary_origin_v` sees through the wrappers: `absolute<celsius>` is `true`, `delta<celsius>` is
  `false` because a delta is an amount, and `kind<Tag, U>` reads as `U` does.
- **A sentence naming the remedy in place of a wall of declined overloads**, for a bare number moved into a quantity, an
  in-place multiply or divide by a quantity, a cross-dimension compound move, and every decibel misuse. On GCC a
  dimensional mismatch is 11 lines and names no declined candidates. Each message is graded by a
  case in `test/errorMessages/`, and the verbatim pages under `docs/diagnostics/` are captured from the compiler and
  re-diffed by `run.py --check-doc`.
- **`scripts/ci_local_msvc.cmd`**, a local mirror of the MSVC CI leg (build + ctest, and `harness` for the
  diagnostic-message suite). `scripts/ci_local.sh` gains the Doxygen build, a markdown-link check, the
  captured-diagnostic diff and a mutation check that every graded message is really the library's own.

### Changed

- **`fdim` and `fmod` answer with an amount.** The positive difference of two readings is an amount and of two decibel
  levels a gain, never another reading or level: `fdim(celsius(30), celsius(10))` is 20 K. Each is available exactly
  where the operator it stands for is: `fdim` where `operator-` is and the operands share a dimension, `fmod` where
  `operator%`'s linear scale is.
- **Rounding into an integer affine target applies the datum.** `round<celsius<int>>(fahrenheit<int>(54))` is 12 °C and
  `round<celsius<int>>(kelvin<int>(300))` is 27, and the result carries the target's own representation.
- **Compound assignment moves a reading by an amount.** `celsius(20) += fahrenheit(9)` is 25 °C and
  `kelvin(300) += celsius(5)` is 305 K: the right operand of a compound move is a relative amount, so only its scale
  factor applies. `dBW += decibels(3.25)` works for the same reason — a gain moves a level as an amount moves a reading.
- **`std::numeric_limits` of a decibel-scale unit reads as finite decibel figures**, built from the stored
  representation: `max()` 3082.547, `epsilon()` 9.643e-16, `min()` −3076.527, `denorm_min()` −3233.062,
  `round_error()` 1.761, `lowest()` −3233.062.
- **`min`, `max` and `clamp` order by magnitude** for a `delta<>`, `absolute<>` or `kind<>` as they do for a plain
  quantity, by comparing the quantities the wrapper holds.
- **`kind<>` delegates its arithmetic to the wrapped unit**, so a tagged quantity answers with the same unit, the same
  representation and the same value the plain quantity does. **`delta<>` scales its own magnitude**, so `delta * scalar`
  and `delta / scalar` agree and a ratio-scaled delta keeps its own unit.
- **`squared`, `cubed` and `square_root` drop the datum**, as their documentation states, so a `sqrt` of a squared
  temperature is a scale-bound magnitude.
- **The Eigen seam matches the scalar rule.** A coefficient-wise difference of readings is an amount, named by a
  `ScalarBinaryOpTraits` specialization, so `(v - w).eval()` on a matrix of equal `celsius` readings is 0. A matrix
  operation is available where the same scalar operation is, and a matrix of plain arithmetic scalars keeps working.
  Assigning a matrix of reading differences back into the reading type is refused, since storing an amount as a reading
  re-applies the datum.
- **A refusal ordinary generic code can encounter is expressed by deleting the overload**, so a `requires`-expression
  observes it and a SFINAE fallback still works: `requires(meters<double> m){ m += 5.0; }` reports `false`. The
  decibel diagnostics instead keep their remedy sentence, which fires from an overload body — so a `requires`-probe
  reports ten decibel operations as available, and generic code that branches on such a probe should ask
  `has_arbitrary_origin_v` or `has_linear_scale_v` instead.
- **Assignment of a bare number to a decibel-scale quantity means decibels**, matching the value constructor, so an
  assign-then-read round trip holds. This reaches `units::decibels` and `dBi`.
- **The transcendental family refuses a logarithmic operand and names the conversion** (`dimensionless(gain)`): `exp`,
  `log`, `log10`, `log2`, `exp2`, `expm1`, `log1p`, `asin`, `acos`, `atan`, `atan2`, `sinh`, `cosh`, `tanh`, `asinh`,
  `acosh`, `atanh`, `sin`, `cos`, `tan`, `sqrt`, `hypot`, `modf` and `fmod`. `sin`, `cos` and `tan` are in that list
  because they take an angle, which is how a dimensionless decibel would otherwise reach the C library through `double`.

### Removed

- **`absolute<>` and `delta<>` around a decibel quantity.** The plain `dBW`/`dBm`/`decibels` types already distinguish a
  level from a gain by dimension, so the wrapper adds nothing and its scaling and magnitude have no single reading.
- **Scaling or dividing a decibel value by a number**, and the transcendental family, `fmod` and `modf` of one. A dB
  figure is a logarithm: scale the linear quantity it denotes, which the message names. The compound and by-value
  forms agree.

### Migration

Everything that stops compiling is decibel, except the Eigen assignment above. Each diagnostic names its remedy:

| stops compiling | write instead |
|---|---|
| `dBW *= 2.0`, `dBW /= 2.0`, `decibels *= 2.0` | scale the linear quantity: `watts(level) * 2.0` |
| `quantity *= decibels(3.0)`, `quantity /= decibels(3.0)` | `quantity *= dimensionless(gain)` |
| a transcendental function, `sqrt` or `hypot` of a decibel value | convert first: `log10(dimensionless(gain))` |
| `fmod` of two same-dimension decibel operands, `modf` of one | `fmod(dimensionless(a), dimensionless(b))` |
| `fdim` mixing a decibel operand with a linear one | give both operands one scale |
| `absolute<dBW<double>>`, `delta<dBW<double>>` | plain `dBW` for a level, `decibels` for a gain |
| `Eigen::Matrix<celsius<double>,3,1> d = v - w;` | −273.15 for equal readings |

These answer differently with no diagnostic, so they are the ones to read:

| expression | 3.6.1 | now |
|---|---|---|
| `kelvin(300) += celsius(5)` | 578.15 K | 305 K |
| `celsius(20) += fahrenheit(9)` | 7.2222 °C | 25 °C |
| `round<celsius<int>>(kelvin<int>(300))` / `(fahrenheit<int>(54))` | 26 / 30 | 27 / 12 |
| the other affine target-unit rounding cells | — | 54 of 100 change value, e.g. `round<fahrenheit<int>>(kelvin<int>(300))` 540 → 80; 10 more change only their result type |
| `decibels g; g = 3.25;` then read | 5.1188 dB | 3.25 dB |
| `numeric_limits<dBW<double>>::max()` / `epsilon()` | `inf` / `0` | 3082.547 / 9.643e-16 |
| `std::hash` of any quantity | of the stored number | of the SI base value |
| `fdim(celsius(30), celsius(10))` / `fmod(...)` | a celsius reading | a kelvin amount |
| `(v - w).eval()` on a matrix of equal `celsius` | −273.15 | 0 |
| `is_affine_unit_v<decltype(celsius(2) * celsius(2))>` | `true` | `false` |
| `kind<"r", kilometers<int>>(3000) + kind<"r", millimeters<int>>(1)` | 3000.000001 km | the plain unit's own answer |
| `delta<percent<int>>(50) * 3.0` | an anonymous dimensionless | `percent<double>` — `.value()` is 1.5 either way |
| `min(grams<signed char>(5), kilograms<signed char>(3))` | −72 g | 5 g |
| `format("{}", fdim(celsius(30), celsius(10)))` | `20 degC` | `20 K` |

An availability sweep over ordinary and affine types reports four changes beyond `lerp` and `midpoint`: `+=` and `-=` of
a bare number stop being *reported* as available — neither ever compiled — and `reading + reading` and cross-unit
`reading + amount` start being available. (`%` and `%=` also lost their floating-point overloads between 3.6.1 and
today, in #404/#408.)


## [3.6.1] - 2026-08-18

### Fixed
- `unit::value()` declared its `needs_fp` compile-time selector as a `static constexpr` local. A `static`
  local in a `constexpr` function is only well-formed from GCC 13 / Clang 17, so GCC 11 and 12 rejected any
  translation unit that instantiated `value()` ("declared 'static' in 'constexpr' function"). The variable is
  used only as a compile-time condition, so dropping `static` is behavior-identical on every compiler while
  restoring the build on GCC 11/12. The declared support floor is unchanged (GCC 13).

## [3.6.0] - 2026-08-17

New capabilities: opt-in affine and string-tagged quantity wrappers, per-dimension concepts, `std::format`
support, floating-point literals with exact compile-time narrowing to integer units, and a set of
correctness and packaging fixes.

### Added
- Opt-in affine wrappers `absolute<Unit>` and `delta<Unit>` (`units/kind.h`, an opt-in header not pulled in
  by `units.h`). `absolute<>` is a point that carries a datum; `delta<>` is an offset-free amount. The
  affine algebra is enforced at compile time: point − point yields a delta, point ± delta yields a point,
  delta ± delta yields a delta, and point + point is ill-formed. Plain `units.h` sees none of this.
- A generic string-tagged quantity kind, `units::kind<"tag", Unit>` (`units/kind.h`). Two kinds with the
  same tag interoperate; different tags are a compile error with a readable diagnostic; a plain unit is
  constructible into a kind by deliberate assignment but does not mix with one in arithmetic. Same-tag
  `kind / kind` yields a dimensionless result. Full parity with a plain unit: `std::hash`,
  `std::numeric_limits`, `abs`/`min`/`max`/`clamp`, and the compound-assignment operators.
- A single conversion verb `to<Target>()` across the wrappers: a plain-unit target unwraps (a point applies
  its datum; a delta or kind scales only), a wrapper target stays wrapped, and an arithmetic target
  (`to<int>`) yields the number.
- A per-dimension concept beside each `is_<dimension>_unit` trait (`units::Velocity`, `units::Force`,
  `units::Length`, `units::Frequency`, `units::Area`, `units::Dimensionless`, …), so a function can
  constrain on a dimension (`void handle(units::Velocity auto v)`) and a computed result classifies the same
  way in every translation unit. (#379)
- `std::format` / `std::print` support for units, with a format specification for the value and the unit
  rendering; `to_string` and `operator<<` are unaffected. Can be disabled with `UNITS_DISABLE_FORMAT`. (#374)
- Unit literals are floating-point, and a compile-time-known value that is exact narrows into an integer
  representation through a `consteval` constructor: `feet<int> f = 16_ft;` compiles while `16.5_ft` is a
  compile error. This extends to a finer integer unit converting into a coarser integer unit when the value
  is an exact whole number of the target (`bytes<int> b = 16_b;` is 2 bytes; `17_b` is a compile error,
  never a silent truncation); a run-time value that need not divide evenly remains rejected. (#375, #380)
- Run-time lossy conversion to a coarser integer unit with explicit rounding intent: `round`, `floor`,
  `ceil`, and `trunc` gain a target-unit overload (`units::floor<bytes<int>>(runtimeBits)`), the same shape
  as `std::chrono::floor<To>`. The rounding is exact integer arithmetic, so it is correct at every magnitude
  (a value beyond 2^53 is not rounded through a lossy double), and a result that does not fit the target
  integer wraps like any integer narrowing rather than invoking undefined behavior. A deliberate value cast
  without rounding remains `unit_cast`. (#375)

### Fixed
- Unit conversions no longer overflow the intermediate computation. An integer conversion carries the
  `value * num` product in a double-width intermediate before the divide, so a large magnitude through a
  fractional ratio yields the correct value instead of a wrapped one; a floating-point conversion divides
  first only when the accurate `value * num` order would overflow to infinity, keeping a finite result
  where one exists. (#387)
- Same-dimension comparison between a signed-underlying and an unsigned-underlying unit is by mathematical
  value, not C++ integer-promotion rules: `meters<int>(-1) < meters<unsigned>(1u)` is now true.
- A cluster of `<cmath>`-analog correctness bugs, including `fma`. (#376)
- Mixed-unit arithmetic reconciles its result to a real named unit and recovers composed and offset-free
  names, computed in the function body rather than the signature. (#381)

### Changed
- Consuming `units` via `add_subdirectory` is now inert: a nested `units` builds no tests, examples, or
  documentation, pulls in no CTest scaffolding (no `BUILD_TESTING` option or `Nightly`/`Continuous`/
  `Experimental` targets in the consumer's cache), and generates no install/export rules — so a consuming
  application's `cmake --install` does not stage `units`' headers or CMake package files. When `units` is
  the top-level project (a CPack build, or a Conan/vcpkg recipe that configures it directly) the install
  rules are on; a subdirectory consumer that wants them can set `UNITS_INSTALL=ON`.
- The continuous-integration suite gained an UndefinedBehaviorSanitizer job, so a conversion that invoked
  runtime undefined behavior fails the build rather than returning a wrong value silently.

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

[3.6.1]: https://github.com/nholthaus/units/releases/tag/v3.6.1
[3.6.0]: https://github.com/nholthaus/units/releases/tag/v3.6.0
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
