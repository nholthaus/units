# AI-issue triage — nholthaus/units v3.6.1 (HEAD aa4c448)

Working notes. NOT staged/committed. All items reproduced against this exact tree
(`clang++ -std=c++23 -I include`; serialization needs c++23 for `std::expected`;
`percent`/`parts_per_million` live in `units::concentration`).

Source: 18 auto-posted "(from Codex)" issues #392–#409 (author Gold856, 2026-08-24),
filed after vendoring the library. UNTRUSTED — triaged skeptically, repro-first.
#378 (Chip Hogg, ODR/UB design discussion) is a real thread already answered by the
maintainer in-comment — NOT part of this AI batch.

## Release-risk lens (the lens that governs at 3.6.1)

This is a mature 3.x line, years in production, vendored downstream. A "bug" that
requires changing **hot, years-shipped public-API semantics** (`operator==`, `std::hash`,
math-function return types) is NOT worth the silent-breakage risk unless it corrupts a
value or crashes. The established behavior IS the spec at this stage. The genuinely
worth-doing work concentrates in the **young opt-in modules** (`eigen.h`, `serialization.h`,
both added in v3.5.0, neither pulled by `units.h`) and the **narrow affine path**, where
fixes are safe and the defects are real.

## Tier A — SAFE to fix (opt-in modules or localized; can't break units.h-only users)

| # | Issue | Repro | Verdict | Fix | Notes |
|---|-------|:----:|---------|:---:|-------|
| 406 | eigen `unit_normalized` dynamic-size → crash / OOB write | YES (aborts, exit 134) | BUG | S | eigen.h:217 default-constructs zero-length result then indexes. `unit_transform` (267/273) has the SAME defect. Highest severity. Fix: size result from `v.size()`. |
| 407 | eigen integral-rep `unit_normalized` truncates ([1,1] not ~[.707,.707]) | YES (partial) | BUG | S/M | eigen.h:215/219 integer sqrt+division. Fix: promote to floating underlying. The issue's `unit_norm` half is INVALID (it's a hard compile error, not silent). |
| 392 | eigen `unit_norm` on percent → 0.5% not 50% | YES | BUG | S | eigen.h:189 uses `.to<Underlying>()` (normalized fraction) not `.raw()` (point count). Only wrong for ratio-dimensionless (percent/ppm/ppb). |
| 396 | serialize neg→unsigned = UINT_MAX; also rejects exact `lowest()` | YES (both) | BUG | S | serialization.h:702 `abs(v) > max()` bounds wrong both ways. Fix: check `[lowest(), max()]`. Untrusted-input hardening. |
| 398 | malformed huge count → `std::length_error` not `deserialize_error` | YES | BUG | S | serialization.h:950 `reserve(count)` before bounding count vs remaining bytes. |
| 403 | decoded denominator 0 accepted (`^1/0`) | YES | BUG | S | serialization.h:958-964 no `den != 0` guard. One-line + maybe an error enumerator. |
| 395 | `visit` default omits `dimension::dimensionless` | PARTIAL | BUG (⅓) | S | Add `dimensionless` to `builtin_dimensions` (serialization.h:73). `angular_acceleration`/`angular_jerk` = INVALID — those dimensions DO NOT EXIST (fabricated by the issue). |
| 401 | formatter separator containing `b` → `0_b_ft` (uninitialized 0 + wrong label) | YES | BUG | M | core.h:6088 flat pre-scan for `b` isn't separator-quote-aware; disagrees with the real parser. Worse than described (corrupt 0 value). |

## Tier B — CARE (real behavior change, narrow surface; test + changelog note)

| # | Issue | Repro | Verdict | Fix | Notes |
|---|-------|:----:|---------|:---:|-------|
| 402 | affine `celsius{20} += fahrenheit{9}` → 7.22°C not 25°C | YES | BUG | S | core.h:3910 coerces RHS to an absolute point, applying F's datum, instead of a delta. Violates the operator's OWN doxygen (delta semantics). CO ruling: `+=` means CHANGE, so 7.22 is wrong. Affine-only surface (temperature etc). |
| 409 | `fdim(celsius{100}, fahrenheit{32})` → −173.15°C not a 100° delta | YES | BUG | S/M | core.h:5674 wraps the diff in an affine `CommonUnit` that reapplies the datum. Same family as 402; the lib's affine `operator-` already does it right (offset-stripped delta). fmax/fmin/fmod/hypot share the shape for affine operands. |
| 404 | serialize→`to<percent>()` → 0.5% not 50% | YES | BUG | S/M | serialization.h:696-705 raw-vs-normalized confusion (same family as 392/408). Round-trip not identity for ratio-dimensionless. Opt-in module. |
| 408 | `percent<int>{50} % ppm<int>{300000}` → 50% not 20% | YES | BUG | M | core.h:4826 dimensionless `%` overload skips the common-unit convert that the dimensioned `%` (4805) does. Narrow (mixed percent/ppm modulo is rare) but it's a core operator. |
| 405 | serialize `uint64 > 2^53` silent loss through double | YES | BUG-ish | S→L | double IS the wire SSOT by design; a "real" fix is a new exact-integer wire path (big). Could instead reject unrepresentable values (small). Opt-in module. Defer. |
| 400 | `floor<bytes<uint64_t>>(bits<uint64_t>{MAX})` wrong | YES (latent) | BUG | S | core.h:5535 raw cast to `widest_signed_int`. CORRECT on `__int128` platforms (this box); WRONG only on MSVC-without-intrinsic fallback. Latent. |
| 399 | stream `deserialize` is O(N²) (slurps whole stream per record) | YES (timed) | BUG (perf) | M | serialization.h:1044 copies entire remaining streambuf each call. Opt-in module. Perf, not correctness. Defer. |

## Tier C — DO NOT TOUCH at this release stage / invalid

| # | Issue | Repro | Verdict | Notes |
|---|-------|:----:|---------|-------|
| 397 | `std::hash` (exact) inconsistent with tolerant `operator==` | YES | ✅ CLOSED (WAD, no change) | The hash hashes the exact value = the standard, correct behavior for float keys. `operator==` is intentionally MORE permissive (tolerant, years-shipped). The `==`-being-looser-than-the-hash causes NO practical harm: two near-equal float values landing in distinct hash buckets is exactly what any float-keyed hash container does (`std::hash<double>` included) — the outcome is identical to the strict-equality behavior everyone already relies on and expects. The `std::` contract is violated only on paper; there is no observable failure mode, no corruption, no surprise beyond ordinary float-hash behavior. Relative-tolerance `==` is provably un-hashable anyway (non-transitive), so exact-value hashing is the only coherent choice. NOT worth a doc caveat, let alone touching the hottest operator at 3.6.1. Close with a short "correct float-hash behavior; tolerant `==` being broader is harmless" note.

**Re the issue's alternative "or define a hash compatible with the chosen equality relation": PROVEN IMPOSSIBLE (non-degenerate).** `==` is a RELATIVE tolerance `|a-b| < eps*|a+b|`, which is NON-TRANSITIVE. Demonstrated on the real code (/tmp/r397t): a chain of 50 adjacent `nextafter` steps from 1.0 are each pairwise `==`, but the endpoints 1.0 and 1.0000000000000111 are NOT `==`. Compatibility (a==b ⟹ h(a)==h(b)) applied along such a chain forces h(endpoint0)==h(endpointN) even though they are unequal; extend the chain across the whole representable range (relative tolerance allows it, hop by hop) and h is forced CONSTANT — every key collides, container ops degrade to O(n). So the only "compatible" hash is the useless degenerate one. A real compatible hash would require making `==` transitive (absolute-grid or exact), i.e. the hot-operator breaking change we've ruled out. The issue's alternative is a mathematical dead end for a relative-tolerance relation. |
| 393 | `hypot` doc says "unit of x" but returns `common_type` | YES | DOC-only | The QUANTITY is correct (5128.98 is the right magnitude in the anonymous common sub-unit; `.value()` on a mixed-unit result is meaningless per Chip Hogg). Changing the return TYPE would silently break every mixed-unit `hypot` caller. Fix: correct the doxygen to say "common type," do NOT touch code. |
| 394 | constexpr `fmax(50_pct,25_pct)` → 0.5% | NO | INVALID | Describes a `.value()` constexpr branch that DOES NOT EXIST — fmax uses `.raw()`, single path. constexpr `fmax` is in fact a hard compile error here (std::fmax not constexpr). Close as invalid. |

## Affine cluster (#402, #409) — tied to the parked absolute/delta work, NOT a standalone main patch

Finding (verified in this clone): `main` (v3.6.1) already carries the KIND foundation (`kind.h`,
`is_losslessly_convertible_unit`), but the `class absolute` (a point, carries datum) / `class delta`
(an amount, offset-free) WRAPPER types are NOT on main — they live only in a PARKED STASH
(`stash@{0}`, "On feat/absolute-delta": a 506-line core.h rework + 33 test lines, never committed,
no remote branch, no open PR). My prior note that "#386 affine+kind is merged" was WRONG for this
tree — only the foundation merged, not the delta wrappers.

Why it matters: #402 (`celsius{20} += fahrenheit{9}` → 7.22 not 25) and #409 (`fdim` reapplies datum)
are exactly the bugs the absolute/delta split fixes STRUCTURALLY — the stash's `operator+=` is typed
`absolute<U> += delta<V>`, so adding an absolute point (the bug) becomes unrepresentable and a delta
carries no datum to misapply. The issues report the BARE-unit affine ops (`celsius<>` with no wrapper);
the delta model supersedes those. So band-aiding #402/#409 on main would be throwaway work the delta
branch reconciles/removes.

DISPOSITION: do NOT hand-patch #402/#409 on main. They belong with the absolute/delta work (revive that
branch, or fix them there). CO: "we WILL do something there" — decision pending on whether to revive the
delta branch vs piecemeal. The bare-affine-op question (does the delta model remove/deprecate bare
`celsius += fahrenheit`, or also correct it?) is a design call for that branch.

## Root-cause families (fixing the pattern clears several)

1. **raw-vs-normalized-fraction** for ratio-scaled dimensionless (percent/ppm/ppb):
   #392, #404, #408 (+ the fmax/fmin/fmod cousins). Wrapping a computed scalar back into
   the unit via `.to()/.value()` (normalized fraction) instead of `.raw()` (point count).
2. **affine operands not routed through the offset-stripping delta path**: #402, #409
   (the lib's affine `operator-` already models it correctly — the others don't reuse it).

## SHIPPED (merged to main)

- **#393** hypot/two-arg-math anonymous unit → LHS-unit-when-lossless. All five (hypot/fmax/fmin/fmod/fdim)
  route through `lhs_result_unit_t` (matches operator+/-); anonymous common unit only when returning LHS would
  truncate an integer. PR #413 (merged 2a149fd). Trap hit + fixed: `lhs_result_unit_t` in the explicit return
  type hard-errors on clang/MSVC for `fmod(double,double)` — use `constexpr auto` + body-computed result unit
  (the operator+ convention). copysign already returned LHS, untouched.
- **#397** hash-vs-tolerant-== → CLOSED WAD (CO closed). #394 constexpr fmax/fmin → CLOSED invalid (test #410).
- **#406/#407/#392** eigen cluster: PR #414 (auto-merge). #406 dynamic-size OOB in unit_normalized AND
  unit_transform → size from runtime size. #407 integral truncation → float-promote. #392 unit_norm raw-vs-value
  → SSOT collapse to `sqrt(unit_squared_norm(v))`. +13 eigen tests, 483/483 all 3 compilers.

## Recommended order (gated on CO personal verification of each repro)

- **P0:** #406 (crash/OOB — safe, opt-in, highest severity).
- **P0:** #402 + #409 (affine delta — CO-confirmed semantics, narrow surface).
- **P1:** #392/#404/#408 (dimensionless raw/value — one conceptual pattern), #396/#398/#403
  (serialization hardening), #401 (formatter), #407 (eigen integral).
- **P2:** #395 (add `dimensionless`), #405/#400/#399 (defer-able).
- **P3 / close:** #397 (design — doc-only at most), #393 (doc-only), #394 (invalid).

Repro TUs in /tmp: r402 r409 r404 r408 r394 r392 r393 r397 (affine/dimensionless),
r405 r400 r396 r398 r403 r399b r395 (serialization), issue406 issue407 issue407b
eigen_issues transform_dyn issue401 (eigen/formatter).
