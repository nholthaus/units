# Numerical scales: linear and decibel

Every `units::unit` carries a third template parameter, its *numerical scale*, which governs how the
stored value relates to the quantity it represents and how arithmetic on that unit behaves. This page
explains the two scales the library ships — `linear_scale` (the default) and `decibel_scale` — and how a
logarithmic (decibel) unit turns addition in dB space into multiplication in linear space.

## The scale parameter

The third parameter of `unit` is the scale:

```cpp
template<ConversionFactorType ConversionFactor,
         ArithmeticType T = UNIT_LIB_DEFAULT_TYPE,
         NumericalScaleType<T> NumericalScale = linear_scale>
class unit;
```

A numerical scale is a policy type with two static member functions, `linearize` and `scale`
(the `is_numerical_scale` requirement in `include/units/core.h`). `linearize` maps a *presented* value (what the user writes and reads) to the *linearized*
value the unit stores internally; `scale` is its inverse, recovering the presented value from storage.
Constructing a `unit` runs the presented value through `linearize`; `raw()` runs the stored value back
through `scale`. For a scale to be usable, `is_numerical_scale_v<Scale, T>` must hold — the concept
`NumericalScaleType<Scale, T>` enforces this.

Storing values *linearized* is what lets one representation serve both scales: conversions, comparisons,
and dimensional arithmetic all operate on the linearized value, so a linear quantity and a logarithmic
quantity of the same dimension remain mutually convertible.

## `linear_scale` — the default

`linear_scale` (`struct linear_scale` in `include/units/core.h`) is the identity: `linearize(v) == v` and `scale(v) == v`. The stored value
*is* the presented value. This is the correct scale for essentially every physical quantity, which is why
it is the default and why you never name it explicitly.

```cpp
#include <units/length.h>

int main()
{
    constexpr units::length::meters<double> m(5.0);
    static_assert(m.raw() == 5.0);   // stored == presented
    static_assert(m.value() == 5.0);
    return 0;
}
```

Because both operations are trivial and `constexpr`, linear units are fully usable in constant
expressions (as the `static_assert`s above demonstrate).

## `decibel_scale` — a logarithmic scale

`decibel_scale` (`struct decibel_scale` in `include/units/core.h`) presents a value in decibels while storing it as a linear ratio:

```cpp
struct decibel_scale
{
    template<class T> static T linearize(const T value) noexcept { return static_cast<T>(std::pow(10, value / 10)); }
    template<class T> static T scale    (const T value) noexcept { return static_cast<T>(10 * std::log10(value)); }
};
```

Writing `dBW(3.0)` linearizes `3` to `10^(3/10) ≈ 1.995` and stores *that*; `raw()` scales it back to
`3` dB. The stored, linearized value is an ordinary linear power ratio, so a decibel unit converts to and
from its linear sibling exactly:

```cpp
#include <units/power.h>
#include <iostream>

int main()
{
    units::power::dBW<double> p(3.0);            // 3 dBW
    units::power::watts<double> w(p);            // convert to linear watts
    std::cout << w.value() << " W\n";            // ~1.995 W
    return 0;
}
```

> **Caveat (not `constexpr`):** `decibel_scale::linearize`/`scale` call `std::pow` and `std::log10`, which
> are not `constexpr`. Consequently a decibel unit **cannot be constructed in a constant expression** —
> `constexpr units::power::dBW<double> p(3.0);` is ill-formed (the constructor calls `linearize`, which
> calls `std::pow`, inside a constant expression). `linear_scale` has no such restriction. Construct
> decibel quantities at run time.

## Decibel arithmetic: `+` in dB is `×` in linear space

The defining property of a logarithmic scale is that adding decibels multiplies the underlying linear
quantities: `10·log₁₀(a) + 10·log₁₀(b) = 10·log₁₀(a·b)`. The library implements `operator+`/`operator-`
for decibel-scaled units accordingly (the `operator+`/`operator-` overloads constrained on `has_decibel_scale_v` in `include/units/core.h`): it multiplies (for `+`) or divides (for `-`)
the *linearized* operands, then re-wraps the result in a decibel-scaled unit.

Because multiplying two power ratios produces a *squared* dimension, adding two dimensioned decibel
quantities yields a squared-dimension result; subtracting them yields a dimensionless dB ratio:

```cpp
#include <units/power.h>
#include <iostream>

int main()
{
    units::power::dBW<double> a(3.0);
    units::power::dBW<double> b(3.0);

    auto sum  = a + b;               // linear: 1.995 * 1.995 -> 6 dB (dimension squared)
    auto diff = a - b;               // linear: 1.995 / 1.995 -> 0 dB (dimensionless)

    std::cout << sum.to<double>()  << " (dB, squared dimension)\n";  // 6
    std::cout << diff.to<double>() << " dB (dimensionless)\n";       // 0
    return 0;
}
```

> **Design rationale:** decibel `+`/`-` do not simply add or subtract the presented dB numbers. Adding
> `3 dBW` and `3 dBW` in the *physical* sense means combining two equal powers, i.e. doubling — which is
> `+3 dB`, giving the magnitude `6`. The multiply-in-linear-space rule reproduces exactly that behavior,
> and it falls out of storing the linearized ratio rather than the dB figure. A dimensioned decibel unit
> may also be combined with a dimensionless dB ratio (`dBi`), scaling the quantity without changing its
> dimension (the mixed dimensioned/dimensionless decibel `operator+`/`operator-` overloads in `include/units/core.h`).

## Detecting a unit's scale

Two traits report which scale a unit carries. Each accepts one or more types and is `true` only when
*every* argument matches:

- `traits::has_linear_scale_v<U...>` (`has_linear_scale` in `include/units/core.h`) — `true` iff every `U` derives from `linear_scale`.
- `traits::has_decibel_scale_v<U...>` (`has_decibel_scale` in `include/units/core.h`) — `true` iff every `U` derives from `decibel_scale`.

```cpp
#include <units/power.h>

int main()
{
    static_assert(units::traits::has_linear_scale_v<units::power::watts<double>>);
    static_assert(units::traits::has_decibel_scale_v<units::power::dBW<double>>);
    // both operands share a scale:
    static_assert(units::traits::has_decibel_scale_v<units::power::dBW<double>,
                                                     units::power::dBm<double>>);
    return 0;
}
```

The decibel `operator+`/`operator-` overloads are constrained on `has_decibel_scale_v`, so they engage
only when both operands are decibel-scaled; linear units use the ordinary arithmetic operators.

> **Note (mixing scales):** `std::common_type` of a linear unit and a decibel unit of the same dimension
> resolves to the *linear* form (the `std::common_type` specialization for `units::unit` in `include/units/core.h`) — linear scale is preferred when the two disagree.

## See also

- [Defining new units](../how-to/defining-new-units.md) — `UNIT_ADD` and `UNIT_ADD_DECIBEL`.
- [Affine temperature scales](affine-temperature.md) — a different non-ratio behavior (datum translation).
- [Type traits reference](../reference/type-traits.md) — `has_linear_scale`, `has_decibel_scale`, and the rest.
- [Internals: named unit types](internals-named-types.md) — why the reverse name map keys on scale as well
  as conversion factor (so `watts` and `dBW` do not collide).
