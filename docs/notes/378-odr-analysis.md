# On the ODR status of the named-result rewrap (#378)

## Setup

Let `E` be a computed quantity expression, e.g. `meters<double>{} / seconds<double>{}`. Its static type in a
translation unit `T` is determined by whether `T` includes the result dimension's header:

- `T` includes `<units/velocity.h>`  ⟹  `decltype(E) = velocity::meters_per_second<double>`  (the *named* form, `N`).
- `T` does not  ⟹  `decltype(E) = unit<conversion_factor<r, velocity_dim>, double>`  (the *base* form, `B`).

Both facts are verified by compilation:

    // T without <units/velocity.h>:  decltype(E) mangles as
    //   units::unit<units::conversion_factor<std::ratio<1,1>,
    //     units::dimension_t<units::dim<length_tag,1>, units::dim<time_tag,-1>>>, double, linear_scale>
    // T with <units/velocity.h>:     decltype(E) mangles as
    //   units::velocity::meters_per_second<double>

## Claim 1 — the differing result type is not, by itself, an ODR violation

ODR ([basic.def.odr]) is violated only when *one entity* has *two differing definitions* across translation
units. `N` and `B` are two *distinct* types (distinct name-mangling, shown above), each with a single
consistent definition. A type used within a translation unit has no cross-unit identity requirement, so
`decltype(E)` denoting `N` in one unit and `B` in another is two different entities, not two definitions of
one. No ODR violation arises from the type divergence alone.

## Claim 2 — an ODR violation is reachable, via a shared inline entity

Let `f` be an inline function (or a non-parameterized entity with external/vague linkage) whose body forms
`E` and is odr-used in two units `T_N` (sees the named form) and `T_B` (does not):

    inline double f() { return handle(E); }   // handle has a better-matching overload for N

`f` has one mangled name in both units but two differing bodies (one calls `handle(N)`, the other
`handle(B)`). This is an ODR violation. It is observable: `f` is emitted as a weak/COMDAT symbol in each
unit; the linker keeps one and discards the other, so both units call whichever body linked. A minimal
reproduction linking `T_N` and `T_B` shows `f` returning the same value from both units — the discarded
body never runs, selected by link order.

Therefore the concern in #378 is correct: a program can contain an ODR violation whose observable effect is
decided by link order.

## Claim 3 — the two forms are observationally equivalent in every channel except type identity

For every computed result, `N` and `B` are equal in each of the following, verified by
`static_assert`/runtime check across the velocity, force, area, and frequency dimensions
(`test/odrDimensionConcept.h`, `OdrEquivalence.NamedAndBaseFormsAreObservationallyEquivalent`):

| Channel | Relation | Basis |
|---|---|---|
| Size | `sizeof(N) == sizeof(B)` | `static_assert` |
| Alignment | `alignof(N) == alignof(B)` | `static_assert` |
| Trivial copyability | `is_trivially_copyable_v<N> == is_trivially_copyable_v<B>` | `static_assert` |
| Object size vs. underlying | `sizeof(N) == sizeof(underlying)` (no hidden state) | `static_assert` |
| Dimension | `same_dimension<N, B>` | `static_assert` |
| Value representation | `memcmp` of the stored value is 0 | runtime |
| Serialized bytes | `serialize(N)` and `serialize(B)` are byte-identical | runtime |
| Hash | `hash<N>(N) == hash<B>(B)` | runtime |

The only relation that does *not* hold is `is_same_v<N, B>` — the type identities differ (Claim 1).

## Consequence — the value is never corrupted; the residual is dispatch selection

The definition folded away in Claim 2 differs from the surviving definition only where the two bodies differ.
By Claim 3 the object representation, value, serialized form, and hash of `N` and `B` are identical, so any
body that merely *carries or computes with* the quantity produces the same result under either form; no value
or dimension is corrupted by the fold.

The residual observable is confined to code that *dispatches on the concrete type identity* — an overload or
specialization keyed on `N`. Such a candidate matches only in a unit that formed `N`, producing the
link-order-dependent selection of Claim 2. Because `N` and `B` mangle distinctly (Claim 1), an entity keyed
directly on `N` (e.g. `std::hash<N>`) yields a distinct, non-colliding symbol and is never folded onto the
`B` entity; the fold of Claim 2 requires a *shared-name* entity (the inline `f`) whose body selects between
`N`- and `B`-keyed candidates.

## Resolution

Dispatch on the dimension, not the concrete result type. Each dimension exposes a concept
(`units::Velocity`, `units::Force`, …) that classifies `N` and `B` identically in every translation unit
(`test/odrDimensionConcept.h`), so an overload/specialization written against the concept selects the same
candidate regardless of includes and admits no differing-body entity. Writing `void f(units::Velocity auto)`
rather than `void f(meters_per_second<double>)` removes the Claim-2 trigger. Including the dimension header of
a result where its type is named keeps result types spelled consistently across a program.

Under this discipline the two forms are interchangeable in every channel a program can observe, and no
odr-used entity has two definitions. The type-identity divergence remains (Claim 1), and is not an ODR
violation on its own.
