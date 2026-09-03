# Changelog

All notable changes to this project are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/), and the project adheres to semantic versioning.

## [Unreleased]

A correctness pass over quantities measured from an arbitrary origin — an affine temperature reading (`celsius`,
`fahrenheit`, `reaumur`) and a decibel LEVEL (`dBW`, `dBm`) — plus the operations that had no overload and reported a
wall of declined candidates instead of a sentence.

The guiding rule is that an operation reads a quantity's number **in the scale that number is written in**, and is
available wherever that reading is well defined. Scaling an affine reading is therefore kept: it is scale-bound, not
wrong, and published temperature formulae depend on it — 16 of the 26 collected in
`test/main.cpp::caseStudyPublishedTemperatureFormulae` have a scalar literal directly multiplying a °C or °F reading.
Magnus's formula has no absolute-scale form at all.

What is ill-formed is an operation with no single reading: a bare number moved into a quantity (it states no unit), and
a transcendental function or a product of a **decibel** value, where the stored number is a logarithm and the ratio it
denotes is a different number.

Every figure below was measured by compiling the expression against 3.6.1 and against this branch. Where an unreleased
change already on `main` is the origin of a difference, it is named rather than claimed here.

### Migration — what stops compiling

A 5,400-probe availability sweep over ORDINARY and AFFINE types — 30 unit types x 5 representations x 36 operations —
reports exactly two changes against `main`: `quantity += <bare number>` stops being *reported* as available (it never
compiled; see Changed), and `reading + reading` starts being available. No ordinary quantity that compiled stops
compiling. (That sweep also shows `%` and `%=` losing their floating-point overloads between 3.6.1 and today, which is
#404/#408 on `main`, not this branch.) A separate 400-probe sweep over the `kind<>`, `delta<>` and `absolute<>`
wrappers reports no change at all.

Everything below is decibel except the last row, which is affine.

| stops compiling | 3.6.1 computed |
|---|---|
| `dBW *= 2.0`, `dBW /= 2.0`, `decibels *= 2.0` | 13.9794 dBW, 7.9588 dBW, 7.7815 dB from `12.5`/`12.5`/`3.0` |
| `quantity *= decibels(3.0)`, `quantity /= decibels(3.0)`, and the same with `percent` | `meters<double>(3) *= decibels(3.0)` → 9 m (an `int` representation gives 8 m); `meters<double>(9) /= decibels(3.0)` → 3 m; `dBW(12.5) *= percent(50)` → 7.9588 dBW |
| a transcendental function of a decibel value | `log10(decibels(3.25))` → 0.5119; `atan(…)` → 1.2723 rad; `atan2(decibels(3), decibels(2))` → 0.9828 rad |
| `fmod` with ANY decibel operand, and `modf` of one | `fmod(dBW(12.5), dBW(4.25))` → 4 and `modf(decibels(3.25))` → a 0.25 fraction against a 4.7712 integral part, both on the dB figures, while `dBW % dBW` was already refused |
| `fdim` of a decibel value against a plain dimensionless | `fdim(decibels(3), dimensionless(2))` → 0 — a difference that `decibels(3) - dimensionless(2)` itself refuses |
| `min`, `max` and `abs` of a `delta<dBW>` | knock-ons of the wrapper's removal: 3.0 and 3 |
| assigning an Eigen matrix of reading DIFFERENCES into the reading type | `Eigen::Matrix<celsius<double>,3,1> d = v - w;` → −273.15 for equal readings; the difference is an amount, and storing it as a reading is what re-applied the datum. `(v - w).eval()` and `v -= w` are unaffected, and the refusal comes from an Eigen-internal `YOU_MIXED_DIFFERENT_NUMERIC_TYPES` assertion rather than a sentence of ours |
| `absolute<dBW<double>>`, `delta<dBW<double>>` | both were valid types |

Each of the decibel refusals reports one sentence naming the remedy. The refusals expressed by DELETING an overload
(see Changed) do not: `meters += 5.0`, `meters *= meters` and `meters += seconds` report a nine-line deleted-function
error naming both operand types and no remedy text, which is the price of making them visible to a `requires`-probe.

### Migration — what silently computes a different answer

These need no code change and raise no diagnostic, so they are the ones to read.

| expression | 3.6.1 | now |
|---|---|---|
| `kelvin(300) += celsius(5)` | 578.15 K | 305 K |
| `celsius(20) += fahrenheit(9)` | 7.2222 °C | 25 °C |
| `round<celsius<int>>(kelvin<int>(300))` | 26 | 27 |
| `round<celsius<int>>(fahrenheit<int>(54))` | 30 | 12 |
| every other affine target-unit rounding cell — 22 more, some large | `round<fahrenheit<int>>(kelvin<int>(300))` 540 → 80; `round<kelvin<int>>(fahrenheit<int>(54))` 30 → 285; `round<celsius<int>>(rankine<int>(500))` 278 → 5; `round<fahrenheit<int>>(celsius<int>(12))` 22 → 54; `round<celsius<int>>(fahrenheit<int>(212))` 118 → 100; `floor`/`ceil`/`trunc<celsius<int>>(fahrenheit<int>(54))` 30/30/30 → 12/13/12; `ceil<celsius<int>>(kelvin<int>(300))` 26 → 27 |
| the RESULT TYPE of all four affine target-unit rounding overloads | `celsius<double>` | `celsius<int>` — the target's own representation |
| `numeric_limits<dBW<double>>::round_error()` / `lowest()`, and the `float` forms | 0.5 / −inf / `max()` inf / `lowest()` −inf | 1.7609 / −3233.06 / 385.318 / −448.535 |
| storing a reading-pair RESULT back into the reading type | `celsius<double> r = fmod(celsius(30), celsius(10))` → 0 | −273.15: the result is an amount, and converting an amount into a reading applies the datum. `= fdim(...)` 20 → −253.15, and `celsius(10) == sqrt(celsius(10)*celsius(10))` true → false |
| the printed form of the retyped results | `format("{}", fdim(celsius(30), celsius(10)))` → `20 degC` | `20 K`; `celsius(10)*celsius(10)` → `373.149… K^2` becomes `100 K^2` |
| `decibels g; g = 3.25;` then read | 5.1188 dB | 3.25 dB |
| `numeric_limits<dBW<double>>::max()` / `epsilon()` / `min()` / `denorm_min()` | `inf` / `0` / `0` / `0` | 3.08e3 / 9.64e-16 / −3.08e3 / −3.23e3 |
| `hash(celsius(0))` vs `hash(kelvin(273.15))` | differ | equal |
| `std::hash<U>` of any quantity | `hash<T>` of the stored number | a mix of the SI base value's bits — every NON-ZERO hash value changes. A quantity whose base value is exactly zero hashes to 0 on both, so `hash(celsius(-273.15))` goes 1.05e19 → 0 |
| `fdim(celsius(30), celsius(10))` | a celsius READING of 20, i.e. 293.15 K | a 20 K amount |
| `fmod(celsius(30), celsius(10))` | a celsius READING of 0, i.e. 273.15 K | a 0 K amount |
| `(v - w).eval()` on an Eigen matrix of equal `celsius` | −273.15 | 0 |
| `is_affine_unit_v<decltype(celsius(2) * celsius(2))>` | `true` | `false` |
| `kind<"r", kilometers<int>>(3000) + kind<"r", millimeters<int>>(1)` | 3000.000001 km | −1294967295 mm, which is what the plain unit already answered |
| `delta<percent<int>>(50) * 3.0` | an anonymous dimensionless, `.raw()` 1.5 | `percent<double>`, `.raw()` 150 — `.value()` is 1.5 either way |

A guard written as `if constexpr (requires(T a){ a * 2.0; })` cannot see the decibel refusals, since those fire from an
overload body — guard on `units::traits::has_arbitrary_origin_v<T>` or `units::traits::has_linear_scale_v<T>` instead.

### Added

- `units::lerp(a, b, t)` and `units::midpoint(a, b)` for any two same-dimension quantities on a linear or affine scale
  (not a decibel pair, whose weighted sum has no single reading). For an affine reading these
  are the datum-independent weighting: a weighted sum of readings is scale-independent exactly when its weights total
  one, so `midpoint(celsius(20), celsius(30))` is 25 °C and the same two temperatures in kelvin give the same physical
  answer, whereas doubling does not. Both delegate to their `std` counterparts, so they inherit exactness at the
  endpoints, monotonicity, and the absence of overflow: `midpoint(meters<int>(INT_MIN), meters<int>(INT_MAX))` is −1 and
  `midpoint(meters<double>(inf), meters<double>(1))` is `inf`.
- `units::traits::has_arbitrary_origin_v<U>` (an affine reading or a decibel level) and
  `units::traits::is_decibel_level_v<U>`, so generic code can ask rather than carry a unit list. Both are total: a type
  that is not a unit at all reads `false` rather than failing to compile.
- **A reading moved by an AMOUNT, by value.** `celsius(20) + kelvin(5)` and `kelvin(5) + celsius(20)` are both 25 °C —
  a reading and an amount commute, and the answer is in the reading's unit — and `reading + reading` reads its right
  operand as an amount on the same terms, which is what the published sum-of-scaled-readings formulae need
  (`0.7*Tnw + 0.2*Tg + 0.1*Ta`, and mean radiant temperature's `(Tg - Ta) * (1 + 0.22*sqrt(v)) + Ta`). None of these
  compiled against 3.6.1.

  **Two readings do NOT commute.** The result takes the LEFT operand's unit and reads the right one as an amount in
  that unit's degrees, so `celsius(20) + fahrenheit(9)` is 25 °C while `fahrenheit(9) + celsius(20)` is 45 °F — 7.22 °C,
  a different temperature. That follows from the same rule rather than contradicting it: nine Fahrenheit-degrees of
  change is five Celsius-degrees, but twenty Celsius-degrees of change is thirty-six Fahrenheit-degrees. Write the
  operand whose scale you want the answer in on the left.
- Diagnostics naming a concrete remedy in place of a wall of declined overloads, for a bare number moved into a
  quantity, an in-place multiply or divide by a quantity, a cross-dimension compound move, and every decibel misuse —
  including the **by-value** `*` and `/`, which 3.6.1 reported as 149 GCC lines with 12 declined candidates. The
  compound form reported nothing at all there — `dBW(12.5) *= 2.0` compiled silently and answered 13.9794 dBW — so both
  spellings are now refused with the same sentence. Each message is graded by a case in `test/errorMessages/`; the
  phrase each case requires is written by hand, while the verbatim pages under `docs/diagnostics/` are captured from the
  compiler and re-diffed by `run.py --check-doc`, so those cannot drift.
- `scripts/ci_local_msvc.cmd`, a local mirror of the MSVC CI leg (build + ctest, and `harness` for the
  diagnostic-message suite). `scripts/ci_local.sh` already existed on `main` (#419) covering the GCC leg, ctest,
  `gen_reference --check`, the dimension drift check and UndefinedBehaviorSanitizer; this adds the Doxygen build, a
  markdown-link check, the captured-diagnostic diff and the mutation check to it. A change to operators, constraints or
  traits should be run through both.

### Changed

- **A refusal that ordinary generic code can encounter is expressed by DELETING the overload, not by a `static_assert`
  in its body.** A body-fired assertion resolves the overload, so the operation is reported as AVAILABLE and generic
  code with a SFINAE fallback hard-errors from inside the library instead of taking its fallback. Measured on 3.6.1,
  `requires(meters<double> m){ m += 5.0; }` was satisfied while the statement itself did not compile; it now correctly
  reports unavailable. What the change costs differs by compiler. Measured through `#include <units.h>`, which fixes the
  instantiation depth the counts depend on: for `meters + seconds` GCC goes from 119 lines and 11 declined candidates to
  11 lines and none, while Clang goes from 58 lines to 97 and from 10 declined candidates to 16, because it lists every
  candidate including the deleted one. The cross-dimension case was already SFINAE-correct in 3.6.1
  (`requires{ a + b; }` and `is_invocable_v<std::plus<>, meters<double>, seconds<double>>` both reported unavailable)
  and stays that way.
- **The decibel diagnostics keep their remedy sentences, and that has a cost worth naming.** They fire from an overload
  body, so a `requires`-probe now reports these operations as AVAILABLE where in 3.6.1 it did not — eight of them, on
  all three decibel types: `dBW * 2.0`, `dBW / 2.0`, `2.0 * dBW`, `2.0 / dBW`, the integral forms, `dBW * dBW`,
  `dBW / dBW`, `atan2(decibels, decibels)`, and — newly, as the price of giving them a remedy at all — `fmod` and `modf`
  of a decibel. `operator%` carries no diagnostic, so it still reports the refusal correctly, which is what the test
  suite pins. Generic code that branches on such a probe will take the wrong branch and
  hard-error from inside the library. The trade buys a one-sentence message (149 GCC lines → 10, 61 Clang lines → 11)
  for operand types that are decibel-specific. `requires{ celsius += 5.0; }` was already reported as available in 3.6.1
  and still is, and the narrowing described above reaches only the 30 dimensioned linear types — a bare number remains
  reported-available-but-uncompilable for the three affine and three decibel types.
- **Adding or subtracting two decibel LEVELS in place reports its own diagnostic.** `dBW += dBW` and `dBW -= dBW` were
  ill-formed in 3.6.1 already, but failed from inside the library rather than at the call site. Each now names the
  remedy: two 10 dBW sources are not a 20 dBW source, so combine the powers in the linear domain, and take a difference
  by value because it is a gain.
- **`kind<>` delegates its arithmetic to the wrapped unit**, so a tagged quantity answers with the same unit, the same
  representation and the same value the plain quantity does — including the plain unit's pre-existing integer overflow
  for a wide unit ratio, which `kind<>` previously masked by promoting to `double`.
- **`delta<>` scales its own magnitude** rather than delegating to the wrapped unit's scaling, so `delta * scalar` and
  `delta / scalar` agree and a ratio-scaled `delta` keeps its own unit.

### Fixed

- **A compound move applied the right operand's datum whenever the LEFT operand carried none.** `kelvin(300) +=
  celsius(5)` read 578.15 K instead of 305: the non-affine `operator+=` takes a non-deduced parameter, so an affine
  right operand converted into the left unit as an absolute READING and brought its datum with it. Twelve combinations
  of `kelvin`/`rankine` against the three affine units were wrong by a whole datum, in `+=` and `-=` alike.
- **Compound assignment of a cross-scale affine quantity applied the right operand's datum** (#402). `celsius(20) +=
  fahrenheit(9)` reinterpreted the right operand as the absolute point −12.78 °C, giving 7.22 °C. The right operand of a
  compound move is a relative *amount*, so only its scale factor applies: nine Fahrenheit-degrees of change is five
  Celsius-degrees, and the result is 25 °C. The same holds for `-=` and for an offset-free right operand.
- **Rounding into an integer affine target applied only the conversion ratio, dropping the datum.**
  `round<celsius<int>>(fahrenheit<int>(54))` read 30 °C instead of 12, and `round<celsius<int>>(kelvin<int>(300))`
  answered 26 for 26.85 °C — the target-unit overloads were gated on a losslessness trait that judges only the RATIO,
  and kelvin and celsius both have ratio 1, so the fractional 273.15 between them was invisible. An affine pair whose
  datums differ now always takes the datum-aware path; a non-affine conversion keeps the exact-integer path unchanged.
- **`fmod` accepted a logarithmic scale, and both `fmod` and `fdim` answered two readings with another reading.** Every
  `operator%` overload requires a linear scale; `fmod` did not, so `fmod(dBW, dBW)` computed on the dB figures while
  `dBW % dBW` was refused. Both also returned the reading type, making `fmod(celsius(30), celsius(10))` a celsius
  READING of 0 — 273.15 K — where the difference of two readings is an amount, and of two decibel levels a gain. For an
  ordinary pair the unit and value are unchanged, including for a narrow integral representation. (The unit these two
  select for an ordinary mixed pair changed in #393, already on `main`, not here.)

  Both are now available exactly where the operator they stand for is. `fdim` computes a difference, so it is callable
  where `operator-` is -- which removed the four mixed decibel/dimensionless pairs it used to answer even though
  `decibels(3) - dimensionless(2)` was already ill-formed. `fmod` computes a remainder, so it requires the linear scale
  `operator%` requires; `%` additionally takes only integral units, being the integral remainder, and that difference
  is deliberate. The equivalence is asserted rather than enumerated, so a future overload cannot drift out of line on
  one side only.
- **`min`, `max` and `clamp` inverted their ordering for a narrow integral operand.** They compared the OPERANDS, which
  routes through `unit::operator<` and reconciles each side in that side's own representation, so a narrow integral one
  wrapped there: `min` and `max` of 5 m against 3 km were SWAPPED, `min(grams<signed char>(5), kilograms<signed char>(3))`
  read −72 g — a negative minimum of two positive masses — and `clamp(meters<int>(5), kilometers(1), kilometers(3))`
  clamped to the upper bound instead of the lower. Ordering in one promoted common unit moves 244 of 3,528 measured grid
  cells from wrong to right and none the other way, and also accepts a `char` or `bool` representation, which
  `unit::operator<` refuses. The operand chosen is still expressed in the unit the existing result rule picks, and that
  rule reads the conversion ratio alone, never whether the value fits — so
  `max(meters<signed char>(5), kilometers<signed char>(3))` now selects 3 km and still cannot hold 3000 in a
  `signed char`. That is a separate rule, shared with `operator+`, `operator-`, `fdim` and `fmod`, and is unchanged.
- **`atan2` answered with the decibel figures.** `atan2(decibels(3), decibels(2))` returned `atan2(3.0, 2.0)` = 0.9828
  where the ratios give 0.8995: it is the one member of the transcendental family taking two arguments, so the unary
  diagnostic macro could not declare it and the C library's `::atan2` claimed the call.
- **The transcendental functions read a decibel figure as if it were the ratio.** `log10(decibels(3.25))` computed
  log10(3.25) = 0.512 where the ratio is 2.113 and its base-ten logarithm is 0.325; `atan(decibels(3.25))` gave 1.2723
  rad where the ratio gives 1.1288. The whole family — `exp`, `log`, `log10`, `log2`, `exp2`, `expm1`, `log1p`, `asin`,
  `acos`, `atan`, `atan2`, `sinh`, `cosh`, `tanh`, `asinh`, `acosh`, `atanh` — plus `sin`, `cos`, `tan`, `modf` and
  `fmod` — now refuses a logarithmic operand and names the conversion (`dimensionless(gain)`). The guard asks whether the
  operand IS logarithmic rather than whether it is provably linear, because the latter is false for anything the trait
  cannot classify, `kind<>`, `delta<>` and `absolute<>` wrappers included: requiring provable linearity withdrew `atan2`
  from 37 wrapper types that had always accepted it.
- **`Eigen` gave the wrong answer for the difference of two readings.** Eigen assumes a coefficient-wise binary
  operation is `op(T,T) -> T`, but the difference of two readings is an offset-free *amount*; assigning it back into the
  reading coefficient re-applied the datum, so `(v - w).eval()` on a matrix of EQUAL `celsius` readings read −273.15
  instead of 0. A `ScalarBinaryOpTraits` specialization names the amount type. The seam is otherwise gated on the
  coefficient's numerical scale, so a matrix operation is available exactly where the same scalar operation is — and a
  matrix of plain arithmetic scalars, which has no scale to disagree with, keeps working.
- **`squared`, `cubed` and `square_root` now drop the datum**, as their documentation already stated. A squared affine
  unit carried celsius's 273.15, which made it compare as affine and re-apply the datum on the way back out. A `sqrt` of
  a squared temperature is consequently a scale-bound magnitude, not a reading.
- **`std::numeric_limits` for a decibel-scale unit returns usable values.** They are built from the stored
  representation instead of being pushed through the value constructor, which linearizes: `max()` was `inf` (violating
  the contract that it is finite), `epsilon()` was `0` (silently zeroing any tolerance written against it), and `min()`
  and `denorm_min()` were both `0`. They read as finite decibel figures, and the standard's `lowest() <= denorm_min()`
  ordering is preserved. A linear scale is unchanged.
- **Assignment of a bare number to a decibel-scale quantity now means decibels, matching the value constructor.**
  `g = 3.25` stored 3.25 as the linear ratio and read back 5.12 dB while `decibels(3.25)` is 3.25 dB, so an
  assign-then-read round trip did not hold. Only a decibel scale is affected.
- **`std::hash` hashed the stored number, so two spellings of one quantity hashed differently.** It hashes the value in
  SI base units, which makes the hash agree with `operator==` across units of the same dimension — eight measured pairs
  that disagreed now agree, among them `meters(1000)`/`kilometers(1)`, `feet(1)`/`inches(12)`, `hours(1)`/`seconds(3600)`
  and `dimensionless(0.5)`/`percent(50)` — and it is usable in a constant expression for a floating-point
  representation, which it was not. Every hash VALUE changes; nothing in the library persists one.

  The base value is mixed as a `double`, and that costs distinctness at extreme magnitudes: adjacent quantities whose
  base exceeds 2^53 in an integral representation collide (32 of 64 adjacent `meters<long long>` pairs above 2^53), and
  a base value past the range of `double` collides with infinity (`meters<long double>(LDBL_MAX)`,
  `kilometers<float>(FLT_MAX)`). Computing the base in `long double` instead removes all of those — 0 of 64 — but
  breaks `hash(celsius(0)) == hash(kelvin(273.15))`, because the extra precision exposes an ULP difference between
  celsius's datum arithmetic and kelvin's literal. Hashing equal values equally is the requirement an unordered
  container relies on; distinctness at 2^53 is quality. So the narrower computation stands, and the collisions above
  are a known limit rather than an oversight.
- **Moving a decibel level by a dimensionless dB gain in place** (`dBW += decibels(3.25)`, and `-=`) now works. The
  by-value `dBW + decibels(3.25)` already did; the compound form was rejected as a dimension mismatch, though a gain is
  a ratio and moves a level exactly as an amount moves an affine reading.

### Removed

- **`absolute<>` and `delta<>` around a decibel quantity.** Both were valid types in 3.6.1; the plain
  `dBW`/`dBm`/`decibels` types already distinguish a level from a gain by dimension, so the wrapper added no information
  and its scaling and magnitude operations had no single reading.
- **Scaling or dividing a decibel value by a number**, and the transcendental family and `fmod` of one. These computed a
  value that was neither reading: the number was read *through* the numerical scale and written back *past* it, so
  `dBW(12.5) *= 2.0` yielded 13.98 dBW — neither the naive 25 dBW nor a linear doubling's +3.01 dB. The by-value
  `dBW * 2.0` never had an overload, so the compound forms now agree with it.

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
