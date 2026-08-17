# ODR / UB Audit — `rewrap_to_named_t` cross-TU type-identity nondeterminism

**Library:** nholthaus `units` (this repo), branch `feat/absolute-delta`.
**Locus:** `include/units/core.h` — present on `main` and all 3.x.
**Status:** confirmed and reproduced on **g++ 15.2** and **clang 21.1**, `-std=c++23`.
**Scope of this document:** AUDIT + CODIFICATION ONLY. No library code was changed. Every
real divergence has a runnable reproduction under `repro/`; run them all with `repro/run.sh`.

---

## 1. Mechanism

Arithmetic on units that yields a *compound / derived* result rewraps that result into its
"canonical named type" (e.g. `meters/seconds -> velocity::meters_per_second`) through

```
detail::rewrap_to_named_t<U>            // core.h:2332 (alias), 2889-2902 (definition)
```

`rewrap_to_named` is SFINAE-driven off an **ADL / `decltype`-only** function:

```
// core.h:2880-2881  — identity fallback (returns void => "no named class"), never defined
template<class ConversionFactor, class Scale>
void named_class_of(ConversionFactor*, Scale*, ...);
```

The *real* overloads of `named_class_of` are emitted, one per named unit, by the
`UNIT_REGISTER_NAMED_CLASS` macro (core.h:300-309), which is invoked from `UNIT_ADD`
(core.h:379-385). Those macro expansions live **in each dimension header**
(`units/velocity.h`, `units/area.h`, `units/force.h`, `units/frequency.h`, …). A registration
is a plain function *declaration* (decltype-only), so it participates in overload resolution
**only in a translation unit that included that dimension's header.**

Therefore, for the SAME expression, `rewrap_to_named_t<...>`:

* resolves to the friendly **named class** (`velocity::meters_per_second<double>`) in a TU that
  included the dimension header, and
* stays the **plain `unit<...>` base** in a TU that did not.

`typeid`-proven (`repro/probe_with.cpp` / `repro/probe_without.cpp`):

```
WITH    <units/velocity.h>:  units::velocity::meters_per_second<double>
WITHOUT it:                  units::unit<units::conversion_factor<std::ratio<1>,
                               units::dimension_t<units::dim<length_tag,ratio<1>>,
                               units::dim<time_tag,ratio<-1>>>>, double, units::linear_scale>
```

When such a result crosses a TU boundary **inside a weak/inline entity** (an `inline` function,
a function template instantiation, a class template member — anything with vague/weak linkage
that the linker deduplicates by name), the one entity now has **two definitions that differ in
a type** — a One Definition Rule violation (`[basic.def.odr]`). The program is ill-formed,
no diagnostic required; in practice the linker silently keeps **one** definition, chosen by
**link order**, and uses it for every caller. Within a single TU nothing is ever inconsistent;
the harm is **cross-TU type-identity nondeterminism**.

### 1.1 A load-bearing structural fact: the layout never diverges

`UNIT_ADD_SCALED_UNIT_DEFINITION` (core.h:210-…) defines a named unit as

```
template<class Underlying = ...> struct meters_per_second : unit<strong, Underlying, scale>
{ /* only defaulted special members + rebind; NO added data members */ };
```

so the named class adds no state to its `unit<...>` base. Measured for
`meters_per_second`, `square_meters`, `newtons`, `hertz`, `joules` at `double`/`float`/`int`
(`repro/s06_probe.cpp`, and each `*_size_probe.cpp`):

```
Named: size=8 align=8 trivially_copyable=1 trivial=1 standard_layout=1
Plain: size=8 align=8 trivially_copyable=1 trivial=1 standard_layout=1   -> IDENTICAL
```

**Consequence:** the ODR violation is *always* a **type-IDENTITY** hazard, **never** an
object-layout / `memcpy` / `sizeof` hazard. A value carried through a divergent-typed struct
field or a value-returning weak inline is **never corrupted** (see `repro/s07` — value stays
2.5 both link orders). The observable harm is confined to surfaces that key on **type identity**:
rendered text, template specialization selection, and overload resolution.

---

## 2. Divergence SOURCES (every `rewrap_to_named_t` site in `core.h`)

| # | Source | core.h line | Example | Named result | Registered in |
|---|--------|-------------|---------|--------------|---------------|
| S1 | `operator*` squared (same-dimension) | 3947 | `m * m` | `area::square_meters` | `units/area.h` |
| S2 | `operator*` compound (cross-dimension) | 3960 | `kg * (m/s^2)` | `force::newtons` | `units/force.h` |
| S3 | `operator/` compound (cross-dimension) | 4091 | `m / s` | `velocity::meters_per_second` | `units/velocity.h` |
| S4 | `operator/` inverse `T / unit` | 4192 | `1.0 / s` | `frequency::hertz` | `units/frequency.h` |
| S5 | `operator/` inverse `dimensionless / unit` | 4146 | `dimensionless / s` | (same as S4) | `units/frequency.h` |
| S6 | `pow<N>` | 4459 | `pow<2>(m)` | `area::square_meters` | `units/area.h` |
| S7 | `sqrt` | 4807 | `sqrt(m^2)` | `length::meters` | `units/length.h` |
| S8 | string paths | 2808, 2819, 2992, 3027 | `to_string`, `operator<<`, `.name()`, `.abbreviation()` | (re-uses S1–S7) | — |
| — | `operator-` inverse (dead) | 4621 | — | — | see note |
| — | ratio-dimensionless `/` | 4159 | `ppb / yr` | (no rewrap) | STABLE by construction |

Notes:
* **S7 `sqrt` is STABLE and cannot be made to diverge** (adversarially verified,
  `repro/sqrt_stable_probe.cpp`). Its rewrap reduces to `length^1 -> length::meters`, whose
  registration lives in `units/length.h`. Two structural facts make that header **unavoidable**
  in any TU that can even form a `length^2` operand: (a) `area.h` hard-includes `length.h`;
  (b) more fundamentally you cannot *name or construct* any `length^2` value (even a plain
  `unit<length^2>`) without `meters` already defined, since the only way to get one is
  `meters*meters`. So every TU that can call `sqrt` on an area already sees the `meters`
  registration — no second type exists. This is the exact asymmetry that separates S7 from
  S1/S2/S6: those rewrap to a named type (`area`/`force`) whose header is **not** a prerequisite
  of the operands, so a TU may legitimately omit it and see the plain base.
* **S5 is not a distinct behavior — it collapses into S4.** `dimensionless / unit` (line 4146)
  and `T / unit` (line 4192) produce the *exact same* two types (`frequency::hertz<double>` vs the
  plain inverse-time `unit<...>`), typeid-confirmed. S5 is therefore N/A as a separate divergence
  and shares every S4 verdict.
* The line-4621 site is spelled `operator-` but carries an inverse-CF result type; it is not a
  reachable subtraction path in normal use and is treated here as covered by the S4/S5 inverse
  analysis (same rewrap target, same verdicts).
* `RatioDimensionless / Dimensioned` (line 4159) returns a plain compound `unit<...>` with **no**
  `rewrap_to_named_t`, so it is STABLE by construction (nothing to diverge).

---

## 3. Observable-harm SURFACES

| Key | Surface | Kind of harm |
|-----|---------|--------------|
| (a) | rendered text (`to_string`, `operator<<`, `.abbreviation()`) | **cosmetic** (value intact) |
| (b) | user template specialization keyed on the named type (custom trait, `std::hash`, `std::formatter`) | **value / behavior** (or compile split) |
| (c) | serialization (`units/serialization.h`) | none — **STABLE** |
| (d) | `std::common_type` / overload resolution | **value / behavior** |
| (e) | result type in a struct field / function signature | **type-only** (no value harm, layout parity) |
| (f) | `sizeof` / `alignof` / `is_trivially_copyable` | none — **STABLE / N-A** (layout parity) |

---

## 4. SOURCE × SURFACE verdict matrix

Legend: **DO** = DIVERGES-observably · **DT** = DIVERGES-type-only · **S** = STABLE · **N/A**.

| Source \ Surface | (a) text | (b) trait/hash/fmt | (c) serialize | (d) common_type/overload | (e) struct/sig | (f) sizeof/align |
|---|---|---|---|---|---|---|
| S1 `m*m` (squared) | **DO** | **DO** | S | **DO** | DT | S |
| S2 `kg*(m/s^2)` (compound) | **DO** | **DO** | S | **DO** | DT | S |
| S3 `m/s` (compound) | **DO** | **DO** | S | **DO** | DT | S |
| S4/S5 `1.0/s` (inverse) | **DO** | **DO** | S | **DO** | DT | S |
| S6 `pow<2>(m)` | **DO** | **DO** | S | **DO** | DT | S |
| S7 `sqrt(m^2)` | **S** | **S** | S | S | S | S |
| S8 string paths | (this IS surface a) | — | — | — | — | — |

* A cell is **DO** where a runnable repro flips output by link order (or splits compile).
* Every **DT** cell is proven by the layout-parity fact in §1.1 + `repro/s07` (`Holder` field): the
  weak symbol `_Z11make_holderdd` is byte-identical in both objects (Itanium ABI does not mangle
  the return type), so two definitions collide under one name with a differing field type — a
  textbook ODR-on-a-type — yet the value survives because layout matches. This is where a *size*
  divergence WOULD miscompile; §1.1 proves no such size divergence exists in this library.
* **(c) serialization is STABLE for every source.** The wire format is keyed off the runtime
  **dimension signature** (base-dimension terms) + the **SI-base magnitude**, computed in
  `serialize()` (serialization.h:894-916) via `detail::signature<Unit>` and `canonical_unit_t<Dim>`
  — never the named type. `any_unit::to_string()` (serialization.h:559-572) renders through
  `visit()`, which resolves the canonical unit by **dimension**, so it prints the dimension form
  (`"2.5 m s^-1"`) even for a named-typed input. Directly verified: serializing the named `mps`
  form and the plain `unit<...>` form produced **identical 25-byte streams and identical
  `to_string()`** (`repro/divc_serial`, `repro/invf_serial`).

---

## 5. Confirmed divergences — minimal reproductions

All commands assume `cd repro/` and `g++ -std=c++23 -I /e/workspace/units/include` (each result
below is identical on `clang++ -std=c++23`). Run everything at once with `./run.sh` (also
`CXX=clang++ ./run.sh`).

### (a) RENDERED TEXT — COSMETIC (value intact)

**S3 `m/s` — `repro/s01_*`**
```
g++ -std=c++23 -I .../include -c s01_a.cpp s01_b.cpp s01_main.cpp
g++ s01_a.o s01_b.o s01_main.o -o s01_ab && ./s01_ab   ->  a=2.5 mps    b=2.5 mps
g++ s01_b.o s01_a.o s01_main.o -o s01_ba && ./s01_ba   ->  a=2.5 m s^-1 b=2.5 m s^-1
```
`describe()` is one weak inline; both `from_a`/`from_b` call it, so the kept definition prints
for both — the link-order flip is the divergence.

**S1 `m*m` — `repro/mul2_text_*`:** `a b` -> `6.25 m2` · `b a` -> `6.25 m^2`
**S2 `kg*(m/s^2)` — `repro/mulc_text_*`:** `a b` -> `20 N` · `b a` -> `20 m kg s^-2`
**S6 `pow<2>(m)` — `repro/pow_text_*`:** `a b` -> `6.25 m2` · `b a` -> `6.25 m^2`

### (b) USER TEMPLATE SPECIALIZATION — VALUE / BEHAVIOR

**S3 custom trait — `repro/s02_*`** (the canonical "42 vs 0" flip)
```
g++ ... s02_a.o s02_b.o s02_main.o -o s02_ab && ./s02_ab   ->  a=42  b=42
g++ ... s02_b.o s02_a.o s02_main.o -o s02_ba && ./s02_ba   ->  a=0   b=0
```
`SpeedPolicy<T>` is specialized only for the named type; the weak `classify()` returns
`SpeedPolicy<decltype(m/s)>::code()`. The dispatched integer flips.

**S3 `std::hash` — `repro/s03_*`:** `a b` -> `0xbeef` (user specialization applies) · `b a` ->
`0x81fb13e9025bfdd4` (library's generic `std::hash<unit>`). A hash-keyed container built on the
result type gets inconsistent hashes across TUs.

**S3 `std::formatter` — `repro/s04_named.cpp` / `repro/s04_plain.cpp`:** a **per-TU COMPILE
SPLIT**, not a link-order flip. The user provides `std::formatter<>` only for the named type and
the library has no built-in formatter for the plain `unit<...>`, so the SAME `std::format("{}", v)`
line **compiles** in a named-visible TU and **hard-errors** in a named-blind TU.

**S1 trait — `repro/mul2_trait_*`:** `a b` -> `77` · `b a` -> `0`
**S2 trait — `repro/mulc_trait_*`:** `a b` -> `98` · `b a` -> `0`
**S6 trait — `repro/pow_trait_*`:** `a b` -> `55` · `b a` -> `0`

### (d) COMMON_TYPE / OVERLOAD RESOLUTION — VALUE / BEHAVIOR

**S3 overload — `repro/s05_*`:** two `tag()` overloads (named vs plain base); weak `dispatch()`
calls `tag(m/s)`. `a b` -> `1` · `b a` -> `2` — the program dispatches to a **different function**
by link order.

**S4 inverse overload — `repro/invf_ctype_*`:** constrained `handle()` overload set keyed on
`is_hertz<T>`; weak `pick_inv()` calls `handle(1.0/s)`. `a b` -> `1` · `b a` -> `2`.

**S3 numeric-value flip — `repro/corr_dispatch_*`** (the sharpest case): a user provides a generic
`handle<U>()` and a more-specialized `handle(meters_per_second<double>)` (idiomatic "speeds get
special handling"). A weak inline `compute()` calls `handle(m/s)` and returns a NUMBER. `a b` ->
`102.5` · `b a` -> `202.5` — a **silent value-level miscompilation**: in the freq/velocity-blind
TU `compute()` runs the generic overload its own source never intended, and the linker's arbitrary
pick decides the whole program's result. Non-standard header names (`corr_dispatch_generic.h`,
`corr_dispatch_velhandle.h`, `corr_dispatch_comp.h`); `run.sh` has a dedicated block for it.

### (e) STRUCT FIELD / SIGNATURE — TYPE-ONLY (no value harm)

**S3 `repro/s07_*`:** `struct Holder { decltype(m/s) v; }` + weak `make_holder()`. The mangled
symbol `_Z11make_holderdd` is identical in both objects yet the field type differs — an ODR
violation on the type. Value stays `2.500` both orders (layout parity, §1.1). `repro/invf_sig_*`
shows the same for the inverse source.

### STABLE (negative) results — proven, not assumed

* **S7 `sqrt`** — `repro/sqrt_stable_probe.cpp` (see §2 note). No second type exists.
* **Serialization** — `repro/divc_serial_*`, `repro/invf_serial_*`: bytes and `to_string()`
  match across TUs and link orders.
* **`divs_stable`** (`repro/divs_stable_*`) — a `/` form that produces a plain compound with no
  rewrap: STABLE.
* **`sizeof`/`align`/`trivially_copyable`** — `repro/s06_probe.cpp` and each `*_size_probe.cpp`:
  named and plain forms are identical for every dimension/underlying checked.

---

## 6. Severity / likelihood per surface

| Surface | Severity | Likelihood | Notes |
|---|---|---|---|
| (a) rendered text | Low (cosmetic) | **High** | Any log/UI printing a computed derived unit across TUs with mixed includes will disagree. No wrong number, just a different spelling. |
| (b) trait / hash specialization | **High** (silent wrong value / dispatch) | Medium | Requires a user type keyed on the named result type. Real when users write `std::hash`/`std::formatter`/policy traits on `meters_per_second` etc. and feed them computed results across TUs. A hash-map keyed on the result type can corrupt lookups. |
| (b) `std::formatter` | Medium (compile error) | Medium | Fails loudly (compile), not silently — but the SAME source line's compilability depends on unrelated include order. |
| (c) serialization | None | — | STABLE by design (dimension-keyed wire form). Safe to rely on. |
| (d) common_type / overload | **High** (silent wrong call target) | Low–Medium | Needs an overload set / `common_type` path that distinguishes named vs plain. Rare in user code, but catastrophic when present (calls the wrong function). |
| (e) struct field / signature | Low (UB, but value-safe here) | Medium | Ill-formed NDR whenever a computed result type is a struct member / signature crossing a TU. Benign *in this library* only because of the layout-parity accident (§1.1); it is still UB and a latent trap if a future named type ever gains a data member. |
| (f) sizeof / layout | None | — | STABLE (layout parity). |

**Value/behavior harm:** surfaces (b) and (d). **Cosmetic-only:** surface (a). **No harm:**
(c) and (f). **Type-only UB (value-safe by accident):** (e).

---

## 7. Blast radius

Realistically, this bites a user whose program is split across translation units — the normal
case for any non-trivial project — where at least one TU forms a derived quantity via `*`, `/`,
or `pow` (e.g. `distance/time`, `mass*accel`, `1/period`) and passes or returns it through an
`inline` function, a function template, or a class template shared with another TU that included
a **different set of dimension headers**. The overwhelmingly common outcome is **cosmetic**: the
same value renders as `"mps"` in one build and `"m s^-1"` in another, flipping with link order —
annoying and confusing but numerically correct. The **dangerous** outcome — a silently wrong
integer, hash, or dispatched function — requires the user to have written a template
specialization or overload set keyed on the friendly named type (`std::hash`/`std::formatter`/a
policy trait/an overload taking `meters_per_second`), which is idiomatic for exactly the users
who lean on the library's named types; for them a computed result can select the specialization
in one TU and miss it in another, and the linker's arbitrary pick decides the whole program's
behavior. Serialization and object layout are **safe** (dimension-keyed / layout-parity), so no
data is corrupted on the wire or in memory. Net: **high likelihood of a cosmetic flip, lower but
non-trivial likelihood of a silent behavioral flip**, and because it is link-order-dependent it
is maddening to diagnose and can change under an innocuous reordering of object files or a new
`#include`. No fix should ship without regression guards covering, at minimum, the (a) text and
(b)/(d) behavioral cells for S1–S6, plus the STABLE guards for S7 and serialization so a fix does
not silently regress them.

---

## 8. Addenda (subagent cross-checks folded in)

* **S5 collapses into S4 — not a distinct divergence.** `operator/(dimensionless, unit)`
  (core.h:4146) and `operator/(T, unit)` (core.h:4192) yield the **exact same** two types
  (`frequency::hertz<double>` vs the plain inverse-time `unit<...>`), typeid-confirmed on both
  compilers. S5 is therefore **N/A** as a separate cell and inherits every S4 verdict.

* **Signature surface (e) is sharper when the divergent type is a PARAMETER, not just a return.**
  `repro/s07` embeds the type in a return/field, and the Itanium ABI does not mangle a return
  type, so both objects export the identical symbol `_Z11make_holderdd` (one name, two field
  types — the ODR-on-a-type). `repro/invf_sig` additionally shows a function whose **parameter**
  is the divergent type: it emits **two genuinely different mangled symbols**
  (`_Z13consume_namedN5units9frequency5hertzIdEE` vs the `…unit…linear_scale…` mangling). So the
  same source can produce either (i) a silent same-name collision (return/field position) or
  (ii) two distinct symbols that simply fail to satisfy each other's calls (parameter position) —
  both are the same underlying type-identity split. Value is intact in both (layout parity, §1.1).

* **Why serialization's `to_string()` is not just STABLE but dimension-formed.** `any_unit::to_string()`
  resolves the canonical unit by walking `visit()`'s built-in dimension candidate set, where the
  base dimensions (`time`, `length`) precede the derived ones (`frequency`, `velocity`); the
  canonical unit of inverse-time is rendered `"s^-1"` and of length/time `"m s^-1"`. So routing a
  computed value through the erased `any_unit` **erases** the named-vs-plain divergence entirely —
  the opposite of surface (a), which calls `units::to_string()` on the concrete (possibly named)
  type and therefore flips. A fix that changes rewrap must preserve this stable serialization
  behavior (regression guard: `repro/divc_serial`, `repro/invf_serial`).
