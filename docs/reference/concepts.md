# Concepts

*The C++20 concept vocabulary the library exposes in namespace `units`, for constraining your own function and class templates.*

Every concept below is a public name in namespace `units` (not `units::traits`), so a template parameter can be written as `template <units::UnitType U>`. Each is a thin wrapper over a [type trait](type-traits.md); the concept form is the idiomatic way to constrain a template, giving cleaner declarations and better diagnostics than an `enable_if`/`static_assert` on the underlying trait.

```cpp
#include <units.h>
using namespace units;
```

## Reference

| Concept | Accepts | Underlying trait |
|---|---|---|
| `ArithmeticType<T>` | any built-in arithmetic type (`std::is_arithmetic_v<T>`) — `int`, `double`, … | `std::is_arithmetic` (`core.h:819`) |
| `NonArithmeticType<T>` | any type that is **not** arithmetic (the complement of the above) | `!std::is_arithmetic_v` (`core.h:826`) |
| `RatioType<T>` | a `std::ratio` specialization | `traits::is_ratio_v` (`core.h:833`) |
| `ConversionFactorType<T>` | a `units::conversion_factor` (the tag that defines a unit's dimension and scale) | `traits::is_conversion_factor_v` (`core.h:840`) |
| `NumericalScaleType<Scale, T>` | a valid numerical-scale policy for representation `T` (has `linearize`/`scale` on `T`) | `traits::is_numerical_scale_v` (`core.h:847`) |
| `UnitType<T>` | any quantity type — an instantiation of `units::unit` (`meters<double>`, `dimensionless<double>`, …) | `traits::is_unit_v` (`core.h:854`) |
| `DimensionedUnitType<T>` | a unit that **has** a dimension (excludes dimensionless) | `is_unit_v && !is_dimensionless_unit` (`core.h:861`) |
| `DimensionlessUnitType<T>` | a **dimensionless** unit (`dimensionless`, `percent`, angle ratios, …) | `is_unit_v && is_dimensionless_unit` (`core.h:868`) |
| `same_dimension<UnitTo, UnitFrom>` | two units of the **same dimension** (mutually convertible, e.g. `meters` and `feet`) | `traits::is_same_dimension_unit` (`core.h:882`) |
| `RatioDimensionlessUnitType<U>` | a dimensionless unit whose conversion ratio is **not** 1 — `percent`, `ppm`, `ppb`, … | `traits::is_ratio_dimensionless_cf_v` (`core.h:1359`) |
| `OrdinaryDimensionlessUnitType<U>` | a dimensionless unit whose ratio **is** 1 — plain `dimensionless` (the complement of `RatioDimensionlessUnitType`) | (`core.h:1361`) |

> **Note:** `NumericalScaleType` takes two parameters — the scale policy and the representation type it must operate on — because a scale's `linearize`/`scale` are checked against a concrete `T`. It appears as the third template parameter of `unit` itself: `NumericalScaleType<T> NumericalScale = linear_scale`. See [numerical scales](../explain/scales.md).

## Constraining on `UnitType`

`UnitType` accepts any quantity of any dimension. Use it for algorithms that are dimension-agnostic — arithmetic that stays within one dimension, min/max, and so on.

```cpp
#include <units.h>

template <units::UnitType U>
U twice(U x) { return x + x; }        // accepts any quantity, preserves its type

template <units::DimensionedUnitType U>
U negate(U x) { return -x; }          // rejects dimensionless quantities
```

## Constraining on `DimensionlessUnitType`

`DimensionlessUnitType` narrows a template to ratios and pure numbers — the quantities that carry a plain value and no dimension. This is the safe place to reach for `.value()` and return a bare `double`.

```cpp
#include <units.h>

template <units::DimensionlessUnitType U>
double as_number(U x) { return x.value(); }   // only well-formed for dimensionless quantities
```

## Constraining on `same_dimension`

`same_dimension<UnitTo, UnitFrom>` expresses that a conversion between two units is meaningful (they share a dimension) without hard-coding either unit. It reads *to, from* — the destination first — matching the direction of an assignment.

```cpp
#include <units.h>

template <class From, class To>
    requires units::same_dimension<To, From>
To convert_to(From x) { return To(x); }       // From and To must share a dimension
```

A complete, self-contained program exercising the four constrained templates above:

```cpp
#include <units.h>

template <units::UnitType U>
U twice(U x) { return x + x; }

template <units::DimensionlessUnitType U>
double as_number(U x) { return x.value(); }

template <units::DimensionedUnitType U>
U negate(U x) { return -x; }

template <class From, class To>
    requires units::same_dimension<To, From>
To convert_to(From x) { return To(x); }

int main()
{
    using namespace units;
    using namespace units::literals;
    using namespace units::length;

    auto              d   = twice(5.0_m);                         // meters<double>, 10 m
    dimensionless<double> r(0.25);
    double            n   = as_number(r);                         // 0.25
    auto              neg = negate(3.0_m);                        // -3 m
    feet              f   = convert_to<meters<double>, feet<double>>(1.0_m);

    (void)d; (void)n; (void)neg; (void)f;
    return 0;
}
```

> **Caveat:** `DimensionedUnitType` and `DimensionlessUnitType` partition the set of unit types — a `UnitType` satisfies exactly one of them. When you overload on the two, the concepts are mutually exclusive, so no ambiguity arises. Likewise `OrdinaryDimensionlessUnitType` and `RatioDimensionlessUnitType` partition the *dimensionless* units (ratio-1 versus not).

## Concepts versus traits

Concepts and the [type traits](type-traits.md) they wrap are two views of the same predicate. Prefer the concept in new code:

```cpp
// modern — concept:
template <units::UnitType U> U f(U x);

// equivalent — trait + SFINAE:
template <class U, std::enable_if_t<units::traits::is_unit_v<U>, int> = 0> U f(U x);
```

The concept form participates in overload resolution more predictably and yields a readable "constraint not satisfied" diagnostic instead of a substitution-failure wall.

## See also

- [Type traits](type-traits.md) — the traits these concepts are built on, plus the ones with no concept form.
- [Numerical scales](../explain/scales.md) — `NumericalScaleType`, `linear_scale`, `decibel_scale`.
- [Cheat sheet](cheat-sheet.md) — the everyday API.
