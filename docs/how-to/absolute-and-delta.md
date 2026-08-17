# `absolute<>` & `delta<>` — point vs. amount, enforced in the type

Some quantities come in two flavours that share a unit but are *not* the same thing:

- a **point** on a scale — `20 °C`, the epoch `2024-01-01`, an absolute pressure, a position;
- an **amount** — a `+10 °C` rise, a `3 h` duration, a pressure *difference*, a displacement.

Adding two points is meaningless (what is "8 a.m. + 9 a.m."?); subtracting them gives an amount ("one hour").
Adding an amount to a point moves the point. The `units` library models this **affine** structure with two
opt-in wrappers:

- **`absolute<U>`** — a point on `U`'s scale. It carries the unit's datum, so converting it applies the offset:
  `absolute<celsius<double>>(0.0).to<kelvin<double>>()` is `273.15 K`.
- **`delta<U>`** — an offset-free amount. Converting it is scale-only, never the datum:
  `delta<celsius<double>>(10.0).to<fahrenheit<double>>()` is an `18 °F` difference, **not** an absolute `50 °F`.

For a non-affine unit (metres, seconds, …) the datum is zero, so a point and an amount coincide numerically; the
wrappers still keep the two roles distinct in the type.

## Opt in — include the header

The wrappers are **strictly opt-in**. They exist only where you include:

```cpp
#include <units/kind.h>
```

Without that include — even with the umbrella `<units.h>` — there is no `absolute`, no `delta`, and no `kind`,
so your own names by those spellings are undisturbed and there is no added cost. You get the wrappers only where
you ask for them.

`absolute`/`delta` sit in an `inline namespace affine` inside `namespace units`, so both spellings name the same
type:

```cpp
#include <units/kind.h>
#include <units/temperature.h>
using namespace units;
using namespace units::temperature;

absolute<celsius<double>>         room(20.0);   // units::absolute
units::affine::delta<celsius<double>> rise(5.0);   // fully qualified — same delta type
```

Use the `affine::` qualifier only to disambiguate `delta`/`absolute` from an identifier of your own.

## The type algebra

The wrappers enforce the affine algebra at compile time:

| expression            | result       | meaning                                             |
|-----------------------|--------------|-----------------------------------------------------|
| `absolute − absolute` | `delta`      | the two datums cancel, leaving an amount            |
| `absolute + delta`    | `absolute`   | move the point up by an amount                      |
| `absolute − delta`    | `absolute`   | move the point down by an amount                    |
| `delta ± delta`       | `delta`      | amounts add                                         |
| `delta * scalar`      | `delta`      | scale an amount                                     |
| `delta / scalar`      | `delta`      | scale an amount                                     |
| `−delta`              | `delta`      | negate an amount                                    |
| `absolute + absolute` | *ill-formed* | the sum of two points has no meaning                |

```cpp
auto span   = absolute<celsius<double>>(100.0) - absolute<fahrenheit<double>>(32.0); // delta, 100 °C-degrees
auto warmer = absolute<celsius<double>>(20.0) + delta<celsius<double>>(5.0);         // absolute, 25 °C
delta<celsius<double>> d(10.0);
d += delta<fahrenheit<double>>(18.0);   // 20 °C-degrees (the °F delta converts by degree size, +10)
d *= 2.0;                               // 40 °C-degrees

// auto bad = absolute<celsius<double>>(20.0) + absolute<celsius<double>>(5.0);      // does NOT compile
```

Compound assignment mirrors the binary operators: `absolute += / -= delta`, `delta += / -= delta`,
`delta *= / /= scalar`.

## The datum rule — points apply the offset, amounts do not

Converting an **absolute** applies the datum; converting a **delta** is scale-only:

```cpp
absolute<celsius<double>>(0.0).to<kelvin<double>>().value();      // 273.15  (offset applied)
absolute<celsius<double>>(0.0).to<fahrenheit<double>>().value();  // 32      (offset applied)

delta<celsius<double>>(10.0).to<kelvin<double>>().value();        // 10      (scale only — a 10° rise)
delta<celsius<double>>(10.0).to<fahrenheit<double>>().value();    // 18      (scale only — 18 °F-degrees)
```

Mixing units across a `+`/`−` respects each operand's role: `absolute<celsius>(20) + delta<fahrenheit>(18)` is
`30 °C`, because the 18 °F *delta* is a 10 °C rise (degree size), added to the 20 °C point.

Non-temperature affine cases work the same way: a pi-carrying angle keeps its factor
(`delta<degrees<double>>(180).to<radians<double>>()` is `π`), and any offset-carrying scale (gauge vs absolute
pressure, an epoch vs a duration) behaves as its datum dictates.

## Result unit — the LHS-unit tie-break

A wrapper arithmetic operator keeps the **left operand's unit**, so `.value()` reads in the unit *you wrote*:

```cpp
(absolute<celsius<double>>(100.0) - absolute<fahrenheit<double>>(32.0)).value();  // 100  (celsius-degrees)
(absolute<fahrenheit<double>>(32.0) - absolute<celsius<double>>(100.0)).value();  // -180 (fahrenheit-degrees)
```

The result is expressed in the left operand's unit — `100` celsius-degrees, not a value in some other unit. The
one adjustment is to the **underlying type**: if the left operand's underlying is integral and cannot hold the
right operand losslessly — e.g. `absolute<kilometers<int>> − absolute<meters<int>>` — the underlying promotes to
floating point while the unit stays kilometres (the delta reads `0.5`). Comparisons reconcile to the common
(finer) unit, so a mixed-integer comparison does not narrow.

## Traits, concepts, math, and formatting

Constrain a template on the role:

```cpp
template<class T> requires traits::is_delta_v<T>    void takes_an_amount(T);
template<AbsoluteType T>                            void takes_a_point(T);   // concept form
// traits::is_absolute_v<T> / traits::is_delta_v<T>; concepts AbsoluteType / DeltaType
```

- `units::abs`, `units::min`, `units::max`, `units::clamp` work on a `delta` (an amount has a magnitude).
- `units::min`, `units::max`, `units::clamp` work on an `absolute` (two points still order).
- Streaming (`operator<<`) and `units::to_string` forward to the wrapped quantity, prefixing a `delta ` marker
  so an amount is visually distinct from a point: `to_string(delta<meters<double>>(3.0))` is `"delta 3 m"`.

## Crossing between types — `to<Target>()`, where the target decides

Every wrapper has one conversion verb, `to<Target>()`, and the **target type you name decides what you get**:

- a **plain-unit** target *unwraps* — you leave the wrapper and get the plain unit (a point applies its datum, a
  delta is scale-only): `absolute<celsius<double>>(0.0).to<kelvin<double>>()` is a plain `273.15 K`;
- a **wrapper** target *stays wrapped* in that role: `.to<absolute<kelvin<double>>>()` stays a point,
  `.to<delta<fahrenheit<double>>>()` stays a delta.

```cpp
absolute<celsius<double>> c(0.0);
c.to<kelvin<double>>();               // plain kelvin (273.15) — unwrapped
c.to<absolute<kelvin<double>>>();     // absolute kelvin — stays a point
c.to<celsius<double>>();              // plain celsius (0.0) — the plain wrapped unit
```

A point cannot `to<>` into a delta (or vice-versa), and neither can become a plain number — those targets have no
overload. This is the only way out of a wrapper: there is no `.quantity()`/`.unwrap()` accessor.

## String-tagged `kind<Tag, U>` — same unit, different *kind* of quantity

Some quantities share a unit **and** a dimension yet are semantically different: a *radial* distance vs. a
*straight-line* distance, a *torque* vs. an *energy* (both newton-metres). `units::kind<"tag", U>` gives each a
distinct type via a compile-time string tag. Different tags never interoperate; the same tag does.

```cpp
#include <units/kind.h>
using namespace units;
using namespace units::length;

kind<"radial",   meters<double>> r(5.0);
kind<"radial",   meters<double>> r2(3.0);
auto sum = r + r2;                 // OK — same kind → radial 8 m
r.to<kind<"radial", feet<double>>>();   // radial feet (tag kept)
r.to<feet<double>>();                    // plain feet (unwrapped, tag dropped)

kind<"straight", meters<double>> s(3.0);
// auto bad = r + s;               // ill-formed: "cannot add two DIFFERENT kinds (… "radial" vs "straight")"
```

- Spell it `units::kind<"tag", U>`.
- A **plain unit is constructible into a kind** by deliberate assignment (`kind<"radial", meters<double>> r =
  someMeters;`) but is **not interchangeable**: mixing a plain unit with a kind in arithmetic (`r + plainMeters`)
  is ill-formed. A value enters a kind only where you name it.
- Two different tags, or a kind mixed with a plain unit, produce a **readable, tag-naming diagnostic**.
- `traits::is_kind_v<T>` / the `KindType` concept classify a kind; `.tag()` reads its tag.

## Guarantees and limits

- **Trivially copyable** and **exactly the size of the wrapped unit** — zero overhead over `U`.
- **`constexpr`** throughout; usable in constant expressions wherever `U` is.
- **No implicit interchange:** an `absolute`, a `delta`, and a `kind` do not implicitly convert to one another or
  to a bare number; the way out is `to<PlainUnit>()`. (A plain unit constructs into a `kind` by explicit
  assignment, as above.)
- **Opt-in, always:** absent `<units/kind.h>` none of these exist. Once you include it and write
  `using namespace units;`, the names `absolute`, `delta`, `kind`, and `fixed_string` are visible at `units::`
  scope; if you have your own identifier by one of those names, qualify with `units::` / `units::affine::`.
- **Serialization** of the wrappers is not yet supported — unwrap with `to<PlainUnit>()` and serialize that.

## Related

- [affine temperature](../explain/affine-temperature.md) — why absolute and difference are different quantities.
- [defining new units](defining-new-units.md) · [math functions](math-functions.md).
