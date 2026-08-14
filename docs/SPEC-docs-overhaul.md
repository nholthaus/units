# Engineering Spec — nholthaus/units 3.x Documentation Overhaul (v3.x → main)

> The on-disk, checklist-driven source of truth for the 3.x documentation overhaul. Detailed and
> checklist-heavy by design — build straight from it, and keep it accurate as work proceeds.

## DECISIONS taken during execution
- **Doxygen doc-comment warnings cleared 70 → 0.** The RECURSIVE fix surfaced 70 pre-existing header
  doc-comment warnings (undefined `@defgroup`s, `\ref <dim>Units`, `@param` on the UNIT_ADD macros,
  `\tparam` spacing, a duplicate section label, an unbalanced `\endcond`/`<tt>`); all fixed as
  comment-only edits (suite still 238 green). The final two — a synthesized `\dir` and a macro-adjacency
  `@param` mis-attribution in `angle.h` — were cleared via Doxyfile config (`SKIP_FUNCTION_MACROS=YES`,
  an unambiguous mainpage path, `MARKDOWN_ID_STYLE=GITHUB`, CHANGELOG in INPUT), no C++ touched. The
  build is now ZERO warnings and `FAIL_ON_WARNINGS` passes, so the docs-CI warning-as-error gate is
  ACTIVE on pull requests.
- **godbolt "Try it live" links: DEFERRED, not faked.** units is not in Compiler Explorer's library list,
  so an embedded-source clientstate link would fail to compile (missing `<units/length.h>`) — a broken
  link undercuts the credibility pitch and violates "every link live." README leads instead with the
  CI-proven in-repo `examples/` (each is built AND run by ctest). Real godbolt links become possible once
  units is added to Compiler Explorer's libraries (a separate upstream PR to the CE project); tracked as a
  future enhancement, not shipped broken.

## SURFACED CODE FINDINGS
- **[FIXED] `minutes` had no registered name/abbreviation** (`include/units/time.h`). `minutes` is
  defined via `UNIT_ADD_WITH_PLURAL_TAG` — which deliberately skips `UNIT_ADD_CONSTANT` (a `min` unit
  constant would collide with the `min` abbreviation) — but that macro ALSO omitted
  `UNIT_ADD_STRONG_CONVERSION_FACTOR` + `UNIT_REGISTER_NAMED_CLASS`, so after named units became classes
  the name lookup couldn't resolve `minutes`: `name()`/`abbreviation()` returned null, it streamed as
  `90 s`, and a direct `name()` deref was UB. Fixed (CO-approved) by adding those two registrations to
  the macro (keeping the constant omitted), + a regression assertion. Committed separately from the docs.
- **[NO CHANGE] Two traits are `units::detail`, not public `units::traits`** — `is_named_unit_v` and
  `is_losslessly_convertible_unit`; `dimension_of_t` is the public alias (no `dimension_of` predicate).
  Docs document them accurately as internal / under real names. No code change (docs-accuracy only).

---

## 0. Context & goal

`nholthaus/units` v3.x is functionally complete and green on GCC-13 / Clang-19 / VS2022, but cannot be
promoted from `v3.x` to the public default branch because **the documentation is a half-updated 2.x
artifact**. 3.x is a *major* version with sweeping renames, a C++14→C++23 uplift, and a headline
feature (readable compiler diagnostics). This spec takes the docs from "lacking" to **above-platinum,
best-C++-library-docs-on-the-internet** for this signature repo, so `main` promotion is justified.

**Prior thread (closed, do not revisit):** the small refactor (deleted `RELEASE_NOTES_v3.4.0.md`;
consolidated all tests into one `test/` folder) landed as PR **#359 — MERGED**, all 3 CI jobs green.

**Locked decisions:**
1. Layout = **README + `docs/` + Doxygen hybrid** (one searchable published site).
2. Scope = **the works** — all content tiers + `examples/`, PLUS a CHANGELOG + FAQ. **No governance
   files** (no CONTRIBUTING / CITATION / SECURITY / CODE_OF_CONDUCT — we yolo it).
3. Pages CD = **build-check on every push/PR (warn-as-error gate); deploy only from `master` + `v*`
   tags.** One-time manual: Settings ▸ Pages ▸ Source = GitHub Actions.

---

## 1. Non-negotiables (binding on all work)

- **ABOVE PLATINUM / SIGNATURE REPO.** Reference-grade; every snippet compiles, every claim is
  source-cited, every link is live, the whole surface polished.
- **Commit as Nic Holthaus, NO AI attribution.** The docs branch commits under
  `Nic Holthaus <nholthaus@gmail.com>`. No "Generated with", no Co-Authored-By, no emoji/tool credit in
  commits, PR, changelog, release notes, or doc prose. Straight human voice.
- **VOICE = std-committee C++ / cppreference register.** NOT nautical. Precise, technical,
  standardese-adjacent; authoritative and exact but legible. Footgun call-outs use cppreference-style
  **Notes / Remarks / Caveats / Gotchas / Precondition / Design rationale** boxes — never
  "danger/shoot yourself," never "smooth sailing."
- **CTAD & ADL explained IN DEPTH, "I hate templates — what does this MEAN for me?"** Accessibility, not
  a softer tone: lead with what you TYPE and GET before any machinery; committee register throughout.
- **PROGRESSIVE DISCLOSURE.** The user-facing "what you type and get" story lives in the FRONT MATTER
  (learn/ + reference/) and stays clean — no template machinery in a first-time user's path. The deep
  "sausage-making" (how the named-type classes, deduction guides, `strong_name`/ADL, #357 fix,
  `named_class_of` reverse-map, hand-defaulted triviality are BUILT) is FULLY documented but DOWNSTREAM,
  in a dedicated, clearly-flagged advanced doc `docs/explain/internals-named-types.md` + a Doxygen
  "Design & Internals" `\page` group, reachable from user docs only as an optional "Under the hood →"
  link. Document everything; confuse no one. `ctad-and-adl-for-humans.md` is the front-matter "what it
  means for you"; `internals-named-types.md` is its "and here's the machinery" companion.
- **NO GOVERNANCE FILES AT ALL.** Do NOT create CONTRIBUTING, CITATION, SECURITY, or CODE_OF_CONDUCT.
  The only Tier-4 deliverables are **CHANGELOG.md** (user-facing release history + the CI release-notes
  source) and **`docs/meta/faq.md`** (user documentation).
- **No public API changes.** Docs only. Allowed non-API edits: Doxyfile, CMake `UNITS_BUILD_DOCS` +
  a docs CI workflow, header `@file` banner text (c++14→c++23), doc-comment additions on headers,
  packaging cruft. If a doc reveals a real code bug, SURFACE it — do not silently patch it under a docs
  PR.
- **Every snippet compiles on C++23.** Prefer extracting from `examples/` so prose can't rot.
- **Branch/PR:** branch `docs/3.x-overhaul` off `v3.x`; PR to `v3.x`; all 3 CI jobs + the new docs job
  green before landing. Do NOT touch `master`/`main`; promotion is the owner's call.

---

## 2. Confirmed API ground truth (source-verified — the facts every doc MUST get right)

Accessors (core.h): **`operator()` is REMOVED** (2.x `unit_t::operator()()` → gone). Use:
- `.raw()` (core.h:2587) — stored value verbatim; `50_pct.raw() == 50`.
- `.value()` (core.h:2599) — scaled/normalized; `50_pct.value() == 0.5`; promotes to FP.
- `.to<T>()` (core.h:2630) — `static_cast<T>`; `.to_linearized()` (core.h:2640); implicit
  `operator Ty()` only for dimensionless (core.h:2681), `explicit` otherwise (core.h:2694).

**2.x→3.x rename table (the migration guide's spine — verified vs `v2.3.5`):**

| 2.x | 3.x | Note |
|---|---|---|
| `unit_t<Units,T,Scale>` | `unit<ConversionFactor,T,NumericalScale>` | 1st param now a `conversion_factor` |
| `meter_t` (singular `_t` alias) | `meters<double>` / `meters<>` / `meters` (CTAD) | `_t` aliases removed |
| `base_unit` / `category` | `dimension` | |
| `unit` (the tag) | `conversion_factor` | |
| `unit_value_t` | *removed* | use `constexpr` unit values |
| `units::math::sqrt` | `units::sqrt` (ADL, unqualified) | `math` namespace removed |
| `cpow` | `pow` | |
| `units::length::meters` | `units::meters` (dimensions inlined) | full ns still resolves ambiguities |
| `x()` (extract value) | `x.value()` / `x.raw()` / `x.to<T>()` | `operator()` removed |
| C++14 | **C++23** | `cxx_std_23` |
| SFINAE `enable_if_t` | **concepts** (`UnitType`, …) | public vocabulary |

**Headline to SELL (front-matter framing):** the USER-FACING payoff is what you get — `meters` is a type
you can write bare (`meters`, `meters<double>`, `meters(5.0)`), it prints as `meters<double>` in errors,
it's a trivially-copyable value with `.value()`/`.raw()`/`name()`. The HOW (class-based named units via
`UNIT_ADD_SCALED_UNIT_DEFINITION` core.h:211 + deduction guides + ADL `strong_name` #357 fix, locked by
the 29-case harness) is the SAUSAGE → `docs/explain/internals-named-types.md`, not the user path.

**CTAD int-vs-double (teach explicitly):** `meters(5)`→`meters<int>`, `meters(5.0)`→`meters<double>`;
`1_m`→`meters<int>`, `1.0_m`→`meters<double>` (guides core.h:248-250; literals core.h:325-336).

**Exact facts pinned for tables:**
- **47 dimension headers** (`include/units/*.h` minus `core.h`), all aggregated by `units.h`:
  acceleration, angle, angular_velocity, area, capacitance, charge, concentration, conductance, current,
  data, data_transfer_rate, density, energy, energy_density, force, frequency, illuminance, impedance,
  inductance, irradiance, jerk, length, luminance, luminous_flux, luminous_intensity,
  magnetic_field_strength, magnetic_flux, mass, power, pressure, radiance, radiant_intensity, radiation,
  solid_angle, spectral_flux, spectral_intensity, spectral_irradiance, spectral_radiance, substance,
  substance_concentration, substance_mass, temperature, time, torque, velocity, voltage, volume.
- **18 physical constants** in `units::constants` (units.h:126-143), ready-made table (symbol/value/what):
  `pi`(1), `c`(299792458 m/s, speed of light), `G`(6.67430e-11, gravitation), `h`(6.62607015e-34, Planck),
  `h_bar`(1.054571817e-34, reduced Planck), `mu0`(1.25663706212e-6, vacuum permeability),
  `epsilon0`(8.8541878128e-12, vacuum permittivity), `Z0`(376.730313668, vacuum impedance),
  `k_e`(8.9875517923e9, Coulomb), `e`(1.602176634e-19, elementary charge), `m_e`(9.1093837015e-31,
  electron mass), `m_p`(1.67262192369e-27, proton mass), `mu_B`(9.2740100783e-24, Bohr magneton),
  `N_A`(6.02214076e23, Avogadro), `R`(8.314462618, gas constant), `k_B`(1.380649e-23, Boltzmann),
  `F`(96485.33212, Faraday), `sigma`(5.670374419e-8, Stefan-Boltzmann).
- **CMake options (exact):** `UNITS_BUILD_TESTS`, `UNITS_BUILD_DOCS`, `UNITS_DISABLE_IOSTREAM`
  (CMakeLists.txt:12-14). README's `-DBUILD_TESTS`/`-DDISABLE_IOSTREAM` are WRONG.

---

## 3. STALE-FIX backlog (the "guilty as charged" list — every item to correct)

Line refs are `README.md` unless noted. ☐ = to fix.

- ☐ L4 tagline "built on C++17" → **C++23** (contradicts the L6 badge + `CMakeLists.txt:30`).
- ☐ L9 "Latest Release - v3.1.0" → current release; drive from version SSOT.
- ☐ L15 download badge says v3.1.0 but **links to `releases/tag/v2.3.1`** → fix link.
- ☐ L17,20 "New in v3.1.0" / "New in v3.0.0" — **empty stubs** → fill (or restructure into CHANGELOG + a
  concise "What's new in 3.x").
- ☐ L24-77 "New in v2.3.1/v2.3.0" — 2.x-era → move to CHANGELOG "Previous releases".
- ☐ L79-84 "Tested on: gcc-7 / clang-5 / msvc2017" → real matrix from the 3 workflow files.
- ☐ L137,168,202 doc-site link `nholthaus.github.io/units` → repoint to the NEW deployed site;
  `namespaceunits_1_1math.html` (L202) references the **removed** math namespace.
- ☐ L286 "value() or operator()" → `operator()` removed; teach `.value()`/`.raw()`/`.to<T>()`.
- ☐ L397 `math::fma(...)` example → **wrong** (math namespace removed); ADL `fma(...)` or `units::fma`.
- ☐ L535-568 unit-definition-macro signatures stale → real 3.x form
  `UNIT_ADD(length, feet, ft, conversion_factor<std::ratio<381,1250>, meters<>>)` (4-arg, no
  singular/plural split; `conversion_factor` not `unit`); `UNIT_ADD_CATEGORY_TRAIT` →
  `UNIT_ADD_DIMENSION_TRAIT` (length.h:84).
- ☐ L609-626 `-DDISABLE_IOSTREAM=ON` / L713-717 `-DBUILD_TESTS=OFF` → `UNITS_*` names.
- ☐ L628 "Improve compilation time" — **empty stub** → fill (subset headers + MSVC flags evidence).
- ☐ L691 `-std=c++17` → `-std=c++23`.
- ☐ L729-754 Build Instructions reference VS2015 / cmake 3.2 / gcc 7 → current toolchains (cmake ≥3.16).
- ☐ L88-132 TOC out of sync (dead `#get-in-touch`, `v2.3.1` anchors) → regenerate for the new structure.
- ☐ Header banners: every `include/units/*.h` + `units.h` `@file @brief` says "c++14" → **c++23**.
- ☐ `docs/Doxyfile.in`: `PROJECT_NUMBER=2.3.0` + brief "c++14" → version SSOT + c++23 (see §6).
- ☐ `debian/changelog` placeholder email `nicolas@example.com`; version `3.1.1` (SSOT); `debian/copyright`
  year 2013-2025 vs LICENSE 2016.
- ☐ `debian/units-docs.docs` references **`README.Debian` which does not exist**; `debian/units.doc-base.ex`
  is unedited placeholder boilerplate → remove/repair so packaging ships nothing bogus.
- ☐ **Version SSOT**: today README(v3.1.0) / download-link(v2.3.1) / CMake(3.4.0) / Doxyfile(2.3.0) /
  debian(3.1.1) all disagree. Make CMake `PROJECT_VERSION` the single source; everything else derives.

---

## 4. NEW-CONTENT backlog (every gap: evidence file:line → the exact artifact to write)

Ranked by user impact. `test/main.cpp` (5622 lines) is the exhaustive example source to mine; each item
names WHERE the material lives and WHAT doc artifact to produce. ☐ per item.

### Tier 1 — flagship, immediate impact
- ☐ **T1.1 Dimension/unit catalog table.** 47 headers (§2 list); per-unit rows from the `UNIT_ADD*` lines
   at each header's top; conversion tests at test/main.cpp:3440-4518. → `docs/reference/supported-units.md`
   (grouped-by-dimension table: dimension · unit · literal · abbreviation) + a Doxygen category index.
- ☐ **T1.2 Physical-constants reference.** 18 constants (§2 table ready). units.h:126-143; tests
   main.cpp:3419,4569. → `docs/reference/constants.md` table (symbol · value · dimension · meaning).
- ☐ **T1.3 `std::chrono` interop.** ctor core.h:2455; `operator duration` core.h:2702; common_type
   core.h:3055; guide core.h:264-268; tests main.cpp:1057,4592. → `docs/how-to/chrono-interop.md` +
   an `examples/chrono.cpp`.
- ☐ **T1.4 NaN/inf + classification + numeric_limits.** predicates core.h:4832,4993; limits
   core.h:4936,4972; tests main.cpp:5087-5160. → `docs/reference/` note + a "special values" how-to.
- ☐ **T1.5 Compiler matrix + C++23 requirement.** from the 3 workflow files; `cxx_std_23`
   CMakeLists.txt:30. → README "Supported compilers" (as version ranges, nlohmann-style).
- ☐ **T1.6 Readable diagnostics AS THE headline feature — with REAL captured error messages.** core.h:
   212-213,2746; harness test/errorMessages/; test main.cpp:1529. The docs MUST show **verbatim, real
   compiler diagnostics** (GCC-13 / Clang-19 / MSVC-2022) for the common mistakes units is DESIGNED to
   reject — never paraphrased or invented — demonstrating the friendly `meters<double>` names. This is
   the v3.4.0 headline PROVEN, not claimed. See **§4a** for the full treatment. → README "error-as-
   feature" block (BAD code + real diagnostic, tabbed per compiler) + `docs/explain/type-safety.md` +
   the cheat-sheet's commented rejection lines.

### §4a — REAL error messages for the mistakes units is DESIGNED to catch (first-class)

> **Wave 2 status:** DONE. E1 (`readable_add_incompatible`) + E7 (`357_ordering`) pre-existed; E2–E6
> added as `readable_wrong_result_type` / `readable_narrowing_to_int` / `readable_scalar_plus_unit` /
> `readable_trig_needs_angle` / `readable_compare_across_dimensions`. Harness now 34/34 green. `run.py
> --emit-doc` captures verbatim per-compiler diagnostics into `docs/diagnostics/` (GCC-13 set committed;
> Clang-19/MSVC captured in the Wave-6 docs CI). NOTE: E5 is `sin(1_m)` (trig needs an angle) — `sqrt`
> of a length is NOT an error (units supports rational dimensions), corrected from the initial guess.
The docs SHOW users **actual, verbatim compiler diagnostics** — captured from GCC-13 / Clang-19 /
MSVC-2022, never paraphrased, invented, or "something like" — for each mistake the library exists to
reject. This is the concrete proof of the type-safety pitch and the v3.4.0 readable-name headline
(the diagnostic names the friendly `meters<double>`, not `unit<conversion_factor<...>>`).

**The canonical "we WANT this to error" cases (each = BAD snippet → real diagnostic):**
- ☐ **E1 add/subtract incompatible dimensions** — `auto x = 1_m + 1_s;` (must name meters & seconds).
- ☐ **E2 wrong assigned result type** — `meters<double> a = 1_m * 1_m;` (that's `square_meters`, not
   `meters`). The dimensional-analysis catch.
- ☐ **E3 narrowing / lossy implicit conversion** — `meters<int> a = 1_ft;` and `meters<int> a = 1.5_m;`
   (integer underlying + lossless-only rule; ties to T5.2).
- ☐ **E4 mixing a bare scalar with a dimensioned unit** — `meters<double> a = 1_m + 5.0;` /
   `double d = 1_m;` (needs `.value()`/`.to<double>()`; dimensionless is the only implicit case).
- ☐ **E5 wrong dimension into a dimensional function** — `sqrt(1_m)` (needs an area) / `sin(1_m)`
   (needs an angle).
- ☐ **E6 comparing/assigning across dimensions** — `if (1_m < 1_kg)` / passing the wrong unit to a
   typed API (the `isMinimumSize(square_feet)` pattern, README:579).
- ☐ **E7 #357-class ordering still compiles** (a POSITIVE case — an expression reducing to a
   not-yet-included dimension builds cleanly; contrast with the errors).

**Presentation:** README error-as-feature block shows 1–2 marquee cases (E1, E2) BAD-code + real
diagnostic **side-by-side**, tabbed per compiler (mp-units style). `docs/explain/type-safety.md` shows
the full E1–E6 set. `docs/reference/cheat-sheet.md` carries commented rejected-code one-liners. Where a
diagnostic is long, show the essential line(s) + note the rest is elided.

**How to get REAL text (no invention) — extend the existing harness:** `test/errorMessages/`
already compiles deliberately-ill-formed `cases/*.cpp` and grades diagnostics (run.py, generate_cases.py;
the E1/E2-style cases largely exist as `readable_*`/`generated_*`). Add a **capture/doc-emit mode** to
`run.py` (e.g. `--emit-doc`) that runs each doc-case through gcc-13/clang-19/cl and writes the trimmed
verbatim diagnostic into a snippet the docs include, so **the shown error can't drift from what the
compiler actually prints** (a doc-drift guard, same spirit as the 29-case harness). Add a doc-case per
E1–E7 if missing. Capture on the real CI toolchains; record which compiler/version produced each shown
block.

### Tier 2 — real features, common needs
- ☐ **T2.1 JSON serialization** (opt-in, `__has_include`, UNTESTED — document the activation contract).
   core.h:5067. → `docs/how-to/json-serialization.md` + `examples/json.cpp` (guarded).
- ☐ **T2.2 `std::hash` / units as container keys.** core.h:4868; tests main.cpp:680,703. →
   `docs/how-to/` + cheat-sheet row.
- ☐ **T2.3 Concepts as public vocabulary.** core.h:819-882 (`UnitType`, `ConversionFactorType`,
   `same_dimension`, …). → `docs/reference/concepts.md` (+ used in the ADL/CTAD & generic-code sections).
- ☐ **T2.4 Complete traits catalog.** core.h:722,769,631,2238,142,2769 + per-dim macro core.h:404-408;
   whole TypeTraits fixture main.cpp:85-593. → `docs/reference/type-traits.md` (full enumerated list).
- ☐ **T2.5 Decibel / non-linear scales.** `linear_scale`/`decibel_scale` core.h:113; `UNIT_ADD_DECIBEL`
   core.h:377; tests main.cpp:3298-3369. → `docs/explain/scales.md` (dB add = linear multiply).
- ☐ **T2.6 Temperature affine/datum.** temperature.h:53-68 (celsius 273.15, fahrenheit offset). →
   `docs/explain/` affine section (absolute vs difference; why not a plain ratio).
- ☐ **T2.7 Installation breadth.** CPack DEB/RPM/TGZ CMakeLists.txt:132-183; exported config
   cmake/unitsConfig.cmake.in + CMakeLists.txt:78-127 (`SameMajorVersion`); PPA debian/changelog. →
   README **integration matrix** (find_package / add_subdirectory / FetchContent / CPM / single-header /
   apt-PPA / DEB / RPM / TGZ) + a "vcpkg/Conan: not yet provided" honest note.
- ☐ **T2.8 natvis visualizer.** natvis/units.natvis; CMakeLists.txt:35-39,92-94. → `docs/how-to/` note
   (auto under MSVC when linking `units::units`) + README mention.

### Tier 3 — config, patterns, migration
- ☐ **T3.1 Config macros + CMake-options table.** `UNIT_LIB_DEFAULT_TYPE` core.h:50; `UNIT_NO_LITERAL_SUPPORT`
   core.h:322; `UNIT_LIB_DISABLE_IOSTREAM` core.h:71; options CMakeLists.txt:12-14. →
   `docs/reference/configuration.md` table.
- ☐ **T3.2 Migration 2.x→3.x.** §2 rename table + notes.txt. → `docs/meta/migrate-v2-to-v3.md` (the
   marquee migration doc: table + per-rename before/after + the `.value()`/`.raw()` caveat +
   `operator()`-removed + `math::`-removed).
- ☐ **T3.3 constexpr patterns.** constexpr sqrt core.h:1928; tests main.cpp:1067. → `docs/explain/` +
   `examples/` static_assert demo.
- ☐ **T3.4 Full `<cmath>` catalog + ADL story.** functions core.h:4420-5013; fixture main.cpp:4631-5152.
   → `docs/how-to/math-functions.md` (list what's wrapped; the ADL "why no `units::` prefix" story).
- ☐ **T3.5 Ternary / `std::common_type`.** core.h:2989-3062; tests main.cpp:593,4653. → cheat-sheet + a
   short explain note.
- ☐ **T3.6 Unit-constant `3 * units::m` idiom.** the `inline constexpr` constants + `operator*`. →
   getting-started "three ways to make a quantity" (explicit / CTAD / literal / `N * unit`).

### Tier 4 — release history + FAQ ONLY (NO governance files)
- ☐ **T4.1 CHANGELOG.md** (Keep-a-Changelog; back-fill 3.0→3.4 from git/releases; future notes derive
   from it — user-facing release history AND the CI release-notes source, matched by `## <version>`).
- ☐ **T4.2 FAQ** (`docs/meta/faq.md`): chrono interop, change default type, int-vs-double, `auto` caveat,
   why conversions are explicit/strict, macro clashes, ambiguous-overload → `units::` qualify; optional
   one-line SemVer/`SameMajorVersion` note.
- ✗ **NO governance files:** do NOT create CONTRIBUTING, CITATION, SECURITY, or CODE_OF_CONDUCT.

### Tier 5 — substantiate claims (honest, don't invent)
- ☐ **T5.1 Performance evidence.** keep/upgrade the x64 disassembly (README:439-458) + a small codegen
   table + a **godbolt link**; if adding compile-time numbers, add a repeatable measurement note.
- ☐ **T5.2 Integer underlying + lossless-vs-truncating rules.** `is_losslessly_convertible_unit`
   core.h:2266; why `meters<int> = 1_ft` is rejected. → caveat box + FAQ.
- ☐ **T5.3 Thread-safety.** trivial value types (main.cpp:799 `trivial`) → one-sentence policy.
- ☐ **T5.4 Exceptions/error policy.** `noexcept`-heavy, compile-time errors → "does not throw" policy line.

---

## 5. Information architecture (the three tiers — exact file list)

### 5.1 `README.md` (front door) — section order
1. logo + badges (CI ×3, license, C++23, version).
2. **Above-the-fold 5-line runnable example + "Try on Compiler Explorer" (godbolt) link.**
3. **Design goals** (intuitive syntax · trivial integration · zero runtime cost · serious testing).
4. **Feature ✓/✗ table.**
5. **"No runtime cost" evidence** (disassembly + codegen table + godbolt).
6. **Type-safety = the product: BAD code + exact `meters<double>` compiler error side-by-side.**
7. **Integration matrix** (T2.7).
8. **Supported compilers** (version ranges) + **C++23** requirement.
9. **At-a-glance supported-units/dimensions table** (condensed; full list in docs/).
10. **What's new in 3.x** (concise; full detail → CHANGELOG + migration).
11. "Where to go next" → docs/ links. (Kill the 29-section monolith.)

### 5.2 `docs/` (in-repo `.md`, Diátaxis-grouped; `docs/README.md` = TOC hub)
- `docs/README.md` — TOC hub (grouped like Catch2).
- **learn/**: `getting-started.md`, `first-quantities.md`, `unit-conversions.md`.
- **explain/**: `why-units.md` (rationale), `dimensional-analysis.md`, `type-safety.md`,
  **`ctad-and-adl-for-humans.md`** (MARQUEE — the "I hate templates" deep-dive; int-vs-double; caveat
  boxes; the FRONT-MATTER "what it means for you"), `efficiency.md`, `scales.md` (linear/decibel),
  `namespaces.md`, `affine-temperature.md`, **`internals-named-types.md`** (the ADVANCED "sausage" —
  clearly flagged optional: class-derives-from-`unit`, deduction guides, `strong_name`/ADL, #357,
  `named_class_of`, hand-defaulted triviality; the companion to ctad-and-adl-for-humans).
- **how-to/**: `defining-new-units.md` (the EASY `UNIT_ADD` way for users; deep `strong_name`/ADL
  mechanics live in `explain/internals-named-types.md`, linked as "Under the hood →"),
  `math-functions.md`, `chrono-interop.md`, `json-serialization.md`,
  `disabling-iostream.md`, `subset-headers-compile-time.md`, `natvis.md`, `cmake-integration.md`.
- **reference/**: **`cheat-sheet.md`** (MARQUEE — QuickRef, task-grouped, inline `// result`, commented
  rejected-code), `supported-units.md`, `constants.md`, `literals.md`, `type-traits.md`, `concepts.md`,
  `configuration.md`.
- **meta/**: `faq.md`, **`migrate-v2-to-v3.md`** (MARQUEE), `limitations.md`. (CHANGELOG at repo root.)

### 5.3 Doxygen (Eigen hybrid, published)
- `docs/*.md` pulled in as `\page` chapters, interleaved with generated class/namespace/unit reference.
- `USE_MDFILE_AS_MAINPAGE = README.md` keeps README as the site landing page.
- Per-dimension headers get `@brief`/`@ingroup` on their units so the reference isn't sparse.
- **A "Design & Internals" `\page` group** collects the advanced machinery (`internals-named-types.md` +
  the `detail`/`strong_name`/deduction-guide reference) — segregated from the user-facing chapters so a
  first-time reader isn't dropped into the sausage; linked from user pages as optional deep-reading.

---

## 6. Doxyfile.in edits (exact) — `docs/Doxyfile.in`

> **Wave 1 status:** all edits below applied AND the template was upgraded to modern format
> (`doxygen -u`, now `# Doxyfile 1.17.0`), which cleared all 94 obsolete-tag warnings. Local doc build
> is green (exit 0) and RECURSIVE=YES now generates **321 HTML pages** (was near-empty — `core.h` +
> all 47 dimension headers now documented). **68 real CONTENT warnings remain, deferred to Wave 6**
> (they're doc-comment bugs, now visible): 9× `@ingroup 'Concepts'` + Prefixes/ConversionFactor/
> Constructors reference undefined `\defgroup`s; 4× `\tparam` missing-whitespace; ~30× unresolved
> `\ref <dim>Units` anchors in the dimension headers. Fix these when Wave 6 defines the group taxonomy
> + `\page` wiring, THEN turn on WARN_AS_ERROR (theme + awesome-css wiring also lands in Wave 6).

- ☑ `PROJECT_NUMBER = @PROJECT_VERSION@` (injected from CMake SSOT; verified generated Doxyfile → 3.4.0).
- ☐ `PROJECT_BRIEF` c++14 → "A compile-time, header-only C++23 dimensional-analysis library."
- ☐ **`RECURSIVE = YES`** (today NO + `INPUT=include` excludes all `include/units/` incl. core.h — the
   site documents almost nothing). Keep `INPUT = README.md include docs` (add docs/ for the `\page` set).
- ☐ `OUTPUT_DIRECTORY` → the CMake **binary dir** (stop polluting the source tree).
- ☐ `GENERATE_LATEX = NO`, `GENERATE_MAN = NO` (HTML only for the site).
- ☐ `WARN_AS_ERROR = @DOXYGEN_WARN_AS_ERROR@` (PR gate sets FAIL_ON_WARNINGS; default off elsewhere).
- ☐ doxygen-awesome-css: `HTML_EXTRA_STYLESHEET`, `GENERATE_TREEVIEW = YES`, `HTML_COLORSTYLE = TOGGLE`,
   extension JS in `HTML_EXTRA_FILES` + custom `HTML_HEADER`.
- ☐ `HAVE_DOT = YES`, `DOT_IMAGE_FORMAT = svg` (graphviz in CI).
- ☐ `EXAMPLE_PATH = examples` (so `@snippet`/`@example` pull from compiling code).
- ☐ Remove dead `MATHJAX_RELPATH`; `HTML_TIMESTAMP = NO`.
- ☐ Decide `WARN_IF_UNDOCUMENTED` — **land OFF** (per-dimension coverage is thin; flip ON later to avoid
   flooding the gate). Record the decision.

---

## 7. `examples/` manifest (compiling SSOT for every snippet)
Each is a tiny `main()` that compiles under C++23 on all 3 toolchains; docs `@snippet` + godbolt-link them.
- ☐ `hello_units.cpp` — 5-line above-the-fold example (bare-name CTAD + `std::cout`).
- ☐ `dimensional_analysis.cpp` — area/velocity derivation; a `static_assert(1_km + 1_m == 1001_m)`.
- ☐ `defining_a_unit.cpp` — `UNIT_ADD` a custom unit + literal (the ADL way).
- ☐ `pythagoras.cpp` — `sqrt(pow<2>(a)+pow<2>(b))` (ADL math).
- ☐ `dimensionless_percent.cpp` — `.value()` vs `.raw()`; dimensionless↔double interop.
- ☐ `chrono_interop.cpp` — units ↔ `std::chrono::duration` both ways.
- ☐ `json_roundtrip.cpp` — guarded by `__has_include(<nlohmann/json.hpp>)`.
- ☐ Build wiring: an `examples/CMakeLists.txt` target (gated by an `UNITS_BUILD_EXAMPLES` option) OR
   extend `test/errorMessages/run.py` to compile-check examples; add a CI step on all 3 workflows.

---

## 8. Doxygen → GitHub Pages CD — `.github/workflows/docs.yaml`
- Model: **official Pages-from-Actions** — `actions/configure-pages@v6` → `upload-pages-artifact@v5` →
  `deploy-pages@v5`, `github-pages` environment, OIDC (NOT legacy peaceiris). `checkout@v4` (match repo).
- Install modern Doxygen via `ssciwr/doxygen-install@v2` + graphviz (apt lags). doxygen-awesome-css as a
  git submodule (recommended) or fetched in-job.
- Build docs through CMake (`-DUNITS_BUILD_DOCS=ON`) so `@PROJECT_VERSION@` injects from the SSOT.
- **Gate on every push/PR** (`DOXYGEN_WARN_AS_ERROR=FAIL_ON_WARNINGS`); **deploy job `if:`** restricted to
  `refs/heads/master` + `refs/tags/v*`.
- `permissions:` contents:read top-level; pages:write + id-token:write on the deploy job.
  `concurrency: {group: pages, cancel-in-progress: false}`.
- ☐ Draft the full YAML in the branch (fill in during Wave 1/6).
- **Manual owner step (mandatory):** Settings ▸ Pages ▸ Source = "GitHub Actions" — deploy fails without
  it. After first successful deploy, **retire `origin/gh-pages`** (stale 2020 site source). No CNAME
  needed for `nholthaus.github.io/units`. (`origin/johelegp-docs` is a prior docs branch — informational.)

---

## 9. Execution waves (build in order; each ends in a real compile/verify gate)

**Branch setup:** ☑ `docs/3.x-overhaul` off `origin/v3.x`; ☑ Nic identity; ☐ grep conflict markers before
every commit.

- **Wave 1 — Foundation & truth-fixes.** ☐ version SSOT; ☐ header c++14→c++23 banners; ☐ Doxyfile.in
  edits (§6); ☐ packaging cruft (§3 debian items). Gate: warm build + Doxygen builds clean locally.
- **Wave 2 — `examples/` first** (§7) + **error-message capture** (§4a). Add/confirm the E1–E7 doc-cases
  in `test/errorMessages/`; add `run.py --emit-doc` to capture verbatim GCC-13/Clang-19/MSVC diagnostics.
  Gate: all examples compile on all 3 toolchains; every E1–E6 case actually FAILS to compile with a
  captured diagnostic that names the friendly type; E7 compiles; CI step added.
- **Wave 3 — README rewrite** (§5.1). Fan subagents over disjoint sections. Gate: every snippet
  `@snippet`s a compiling example; every link resolves; no stale strings remain.
- **Wave 4 — `docs/` set** (§5.2). Fan subagents over disjoint `.md`, each mining cited source. Marquee
  files reviewed hardest. Gate: snippets compile; voice = committee register; CTAD/ADL & caveat boxes
  present.
- **Wave 5 — CHANGELOG + FAQ ONLY** (no governance files). Gate: CHANGELOG.md valid (Keep-a-Changelog,
  `## <version>` headings the CI release job matches) + `docs/meta/faq.md` present; NO CONTRIBUTING /
  CITATION / SECURITY / CODE_OF_CONDUCT created.
- **Wave 6 — Doxygen `\page` wiring + CD** (§5.3, §8). Gate: site builds coherently locally; `docs.yaml`
  passes on the branch (build-only, no deploy).
- **Wave 7 — Whole-surface adversarial QA.** Fresh subagents re-verify: every snippet compiles, every
  claim source-cited, every link live, register correct, no nautical, no AI attribution, versions agree.
  Then rebuild + full suite (238) + harness (29) + examples + docs build ALL green. → open PR to `v3.x`.

---

## 10. Verification (end-to-end, before PR)
- ☐ `grep -rniE 'c\+\+1[47]|v3\.1\.0|gcc-7|clang-5|msvc2017|DISABLE_IOSTREAM=|BUILD_TESTS=|math::' README.md docs/`
  returns nothing legitimate (all stale strings gone).
- ☐ All 5 version-bearing files agree (README / Doxyfile / CMake / debian / badges) — one SSOT.
- ☐ `examples/*` compile on GCC-13 + Clang-19 + MSVC-2022; the 238-test suite + 29-case harness green.
- ☐ Doxygen builds with zero warnings (WARN_AS_ERROR); `\page` chapters + generated reference both render;
  per-dimension units appear (RECURSIVE fix) and are `@ingroup`'d.
- ☐ Every shown compiler error is REAL captured text (§4a): each E1–E6 doc-case fails to compile on its
  named toolchain and the doc block matches the harness capture (no invented/paraphrased diagnostics);
  E7 compiles. The shown diagnostics name friendly types (`meters<double>`), proving the headline.
- ☐ A fresh reader can install via each documented path, write hello-units, understand 2.x→3.x, and know
  the 3.x idioms (bare names, `.value()`/`.raw()`, ADL math) from the rendered docs alone.
- ☐ PR to `v3.x`: all 3 build jobs + the new docs build job green. `docs.yaml` deploy is gated (won't
  publish until `master`/tag). Commits authored by Nic Holthaus; no AI attribution anywhere.
