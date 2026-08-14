# Migrating from 2.x to 3.x

*The 3.x line is a major revision: it requires C++23, renames several core types, and removes a few
things. This guide lists every change that affects source code, with the before/after for each. Most
everyday code — literals, arithmetic, conversions — is unchanged; the renames are concentrated in the
type spellings and the value-extraction call.*

## The one-minute summary

| 2.x | 3.x | What to do |
|---|---|---|
| C++14 | **C++23** | Compile with `-std=c++23` (or `/std:c++latest` on MSVC). |
| `meter_t`, `second_t`, … (singular `_t` aliases) | `meters<double>` or `meters` | Replace `meter_t` with `meters` (CTAD) or `meters<double>`. |
| `x()` to read the value | `x.value()`, `x.raw()`, or `x.to<T>()` | Replace `x()` with `x.value()`. `operator()` is removed. |
| `units::math::sqrt(x)` | `sqrt(x)` (or `units::sqrt(x)`) | Drop the `math::`; the functions are found by ADL. |
| `units::length::meters` | `units::meters` also works | Optional — the dimension namespaces are now inline. |
| `unit_t<...>` | `unit<...>` | The class template was renamed (first argument is now a `conversion_factor`). |
| `base_unit` / category | `dimension` | Renamed throughout. |
| `unit` (the tag type) | `conversion_factor` | Renamed. |
| `unit_value_t` | *removed* | Use a `constexpr` quantity value instead. |
| `cpow` | `pow` | Renamed. |

The rest of this page expands each of these.

## The C++ standard

3.x requires **C++23**. The 2.x line targeted C++14. There is no configuration that makes 3.x build on an
older standard; the class-based named types, the concepts, and the `constexpr` math all depend on it. If
you cannot move to C++23, stay on the 2.x series.

## Type spellings: `meter_t` → `meters`

In 2.x, the common spelling for a `double`-backed quantity was the singular alias `meter_t`. Those
aliases are gone. Use the plural class template, with or without an explicit representation:

```cpp
// 2.x
meter_t distance = 5.0_m;

// 3.x
meters distance = 5.0_m;        // CTAD deduces meters<double>
meters<double> distance = 5.0_m; // equivalently, spelled out
```

`meters<double>` and `meters` name the same type. See
[CTAD and ADL](../explain/ctad-and-adl-for-humans.md).

## Reading the value: `operator()` → `.value()`

The biggest source-level change. In 2.x you extracted the underlying value by calling the quantity;
`operator()` no longer exists.

```cpp
meters d = 5.0_m;

// 2.x
double v = d();

// 3.x
double v = d.value();   // scaled value
double r = d.raw();     // stored value (differs from value() for percent/ppm — see the FAQ)
double t = d.to<double>();
```

For most units `value()` and `raw()` return the same number; they differ for ratio-dimensionless units
such as `percent` (`50_pct.value() == 0.5`, `50_pct.raw() == 50`) — see [the FAQ](faq.md).

## Math functions: `units::math::` → `units::` (ADL)

The `units::math` namespace was removed. The unit-aware `<cmath>` functions now live in `units` and are
found by argument-dependent lookup, so you call them unqualified:

```cpp
// 2.x
auto h = units::math::sqrt(a * a + b * b);

// 3.x
auto h = sqrt(a * a + b * b);   // found by ADL; or units::sqrt(...)
```

## Namespaces: dimensions are now inline

The per-dimension namespaces (`units::length`, `units::mass`, …) still exist, but they are *inline*
within `units`, so the short form works:

```cpp
units::length::meters m1(1.0);   // still valid
units::meters         m2(1.0);   // now also valid
```

The dimension namespace is still useful to disambiguate a unit name shared across dimensions — for
example `units::mass::pounds` versus `units::force::pounds`.

## Renamed core machinery

If your code names the library's internal vocabulary (most application code does not), note:

- `unit_t<Units, T, Scale>` → `unit<ConversionFactor, T, NumericalScale>`. The first template argument is
  now a `conversion_factor`, not a "units" tag.
- `base_unit` (and "category") → `dimension`.
- `unit` (the old tag type) → `conversion_factor`.
- `cpow` → `pow`.

## Removed: `unit_value_t`

The compile-time `unit_value_t` mechanism is gone. Its use case — a value known at compile time — is
served directly by a `constexpr` quantity, since the quantity types are themselves `constexpr`-friendly:

```cpp
// 2.x: unit_value_t<meters, 5>
// 3.x:
constexpr meters five = 5.0_m;
static_assert(five == 5.0_m);
```

## What did not change

Literals (`5.0_m`, `60.0_mi`, `1.0_hr`), arithmetic and its dimensional results, implicit lossless
conversions, `std::cout <<`, `to_string`, and the trait-based programming style all work as before. Much
2.x code compiles under 3.x after the `_t`→plural and `()` → `.value()` substitutions plus the C++23 switch.

## New in 3.x

Beyond the renames, 3.x adds capabilities: readable compiler
diagnostics ([type safety](../explain/type-safety.md)), `std::chrono` interop
([chrono](../how-to/chrono-interop.md)), a concept vocabulary for your own templates, `std::hash` and
`std::numeric_limits` support, NaN/infinity handling, optional
[JSON serialization](../how-to/json-serialization.md), and the
[Visual Studio debugger visualizer](../how-to/natvis.md). See the [changelog](https://github.com/nholthaus/units/blob/master/CHANGELOG.md).
