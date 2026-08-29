# Type traits

*The `units::traits` catalog — the compile-time predicates and accessors used to introspect unit types, group by the family they belong to.*

Every public trait lives in namespace `units::traits`. Predicate traits derive from `std::bool_constant` and provide a `_v` value alias; accessor traits expose member typedefs and, where an alias is provided, a `_t` form. The examples assume:

```cpp
#include <units.h>
#include <type_traits>
using namespace units;
using T = units::length::meters<double>;
```

> **Note:** The modern way to *constrain a template* on any of the predicate traits below is a [concept](concepts.md) (`units::UnitType`, `DimensionlessUnitType`, `same_dimension`, …), not a raw `static_assert`. Reach for a trait when you need the boolean in a `constexpr` expression, an `if constexpr`, or a diagnostic `static_assert`; reach for the concept when you are declaring a template parameter.

## Structural traits

Predicates on the *shape* of a type — is it a unit, a conversion factor, a ratio, a numerical scale.

| Trait (`_v` form) | Meaning |
|---|---|
| `is_unit<T>` / `is_unit_v<T>` | `T` is a `units::unit` instantiation (a quantity type). `false` for a bare arithmetic type. (`is_unit` in `include/units/core.h`) |
| `is_conversion_factor<T>` / `is_conversion_factor_v<T>` | `T` is a `units::conversion_factor` (the tag defining a unit's dimension and ratio). (`is_conversion_factor` in `include/units/core.h`) |
| `is_ratio<T>` / `is_ratio_v<T>` | `T` is a `std::ratio` specialization. (`is_ratio` in `include/units/core.h`) |
| `is_numerical_scale<Scale, T>` / `is_numerical_scale_v<Scale, T>` | `Scale` is a valid numerical-scale policy for representation `T` (has `linearize`/`scale` on `T`). (`is_numerical_scale` in `include/units/core.h`) |

```cpp
static_assert(traits::is_unit_v<T>);
static_assert(traits::is_conversion_factor_v<T::conversion_factor>);
static_assert(traits::is_ratio_v<std::ratio<1, 2>>);
static_assert(traits::is_numerical_scale_v<linear_scale, double>);
```

> **Note:** Whether a unit is a *named* type (`meters`) as opposed to an anonymous `unit<…>` computed by arithmetic is answered by `units::detail::is_named_unit_v<T>` (`is_named_unit_v` in `include/units/core.h`). This lives in `units::detail` and is an internal implementation trait, not part of the public traits vocabulary — do not depend on it in application code.

## Dimensional traits

Predicates on a unit's *dimension* — which physical quantity it measures, and whether two units are compatible.

| Trait (`_v` form) | Meaning |
|---|---|
| `is_same_dimension_unit<U1, U2>` / `is_same_dimension_unit_v<U1, U2>` | `U1` and `U2` are units of the same dimension (mutually convertible). (`is_same_dimension_unit` in `include/units/core.h`) |
| `is_dimensionless_unit<T>` / `is_dimensionless_unit_v<T>` | `T` is a dimensionless unit (`dimensionless`, `percent`, an angle ratio, …). |
| `is_<dimension>_unit<T>` / `is_<dimension>_unit_v<T>` | `T` is a unit of the named dimension — one member of a generated family (see below). (`UNIT_ADD_DIMENSION_TRAIT` in `include/units/core.h`) |

```cpp
static_assert(traits::is_same_dimension_unit_v<units::length::meters<double>,
                                               units::length::feet<double>>);
static_assert(traits::is_dimensionless_unit_v<units::dimensionless<double>>);
static_assert(traits::is_length_unit_v<T>);
static_assert(traits::is_area_unit_v<units::area::square_meters<double>>);
static_assert(!traits::is_area_unit_v<T>);
```

### The per-dimension `is_<dimension>_unit` family

`UNIT_ADD_DIMENSION_TRAIT(<dimension>)` (`UNIT_ADD_DIMENSION_TRAIT` in `include/units/core.h`) generates a matching `traits::is_<dimension>_unit<T>` predicate (with its `_v`) for each dimension the library ships. Include the dimension's header to make its trait available. The generated members are:

`is_length_unit`, `is_mass_unit`, `is_time_unit`, `is_angle_unit`, `is_current_unit`, `is_temperature_unit`, `is_substance_unit`, `is_luminous_intensity_unit`, `is_solid_angle_unit`, `is_frequency_unit`, `is_velocity_unit`, `is_angular_velocity_unit`, `is_acceleration_unit`, `is_force_unit`, `is_pressure_unit`, `is_charge_unit`, `is_energy_unit`, `is_power_unit`, `is_voltage_unit`, `is_capacitance_unit`, `is_impedance_unit`, `is_conductance_unit`, `is_magnetic_flux_unit`, `is_magnetic_field_strength_unit`, `is_inductance_unit`, `is_luminous_flux_unit`, `is_illuminance_unit`, `is_radioactivity_unit`, `is_torque_unit`, `is_area_unit`, `is_volume_unit`, `is_density_unit`, `is_concentration_unit`, `is_data_unit`, `is_data_transfer_rate_unit`, `is_dimensionless_unit`, `is_substance_concentration_unit`, `is_substance_mass_unit`, `is_jerk_unit`, `is_energy_density_unit`, `is_luminance_unit`, `is_irradiance_unit`, `is_radiance_unit`, `is_radiant_intensity_unit`, `is_spectral_flux_unit`, `is_spectral_intensity_unit`, `is_spectral_irradiance_unit`, `is_spectral_radiance_unit`.

Each takes one type and is `true` iff it is a unit of that dimension:

```cpp
static_assert(traits::is_mass_unit_v<units::mass::kilograms<double>>);
static_assert(traits::is_velocity_unit_v<units::velocity::meters_per_second<double>>);
```

### Lossless convertibility

Whether a conversion between two units loses no precision (relevant to integer representations, where an implicit conversion is disallowed if it would truncate) is decided by `units::detail::is_losslessly_convertible_unit<UnitFrom, UnitTo>` (`is_losslessly_convertible_unit` in `include/units/core.h`). This is an internal `units::detail` trait — the *observable* rule it enforces is that a lossy implicit conversion into an integer representation is ill-formed. Rely on that compile-time rejection rather than testing the internal trait directly.

## Scale traits

Which [numerical scale](../explain/scales.md) a unit carries — linear (the default) or logarithmic (decibel). Each accepts one or more types and is `true` only when *every* argument matches.

| Trait (`_v` form) | Meaning |
|---|---|
| `has_linear_scale<U...>` / `has_linear_scale_v<U...>` | every `U` uses `linear_scale`. (`has_linear_scale` in `include/units/core.h`) |
| `has_decibel_scale<U...>` / `has_decibel_scale_v<U...>` | every `U` uses `decibel_scale`. (`has_decibel_scale` in `include/units/core.h`) |

```cpp
static_assert(traits::has_linear_scale_v<T>);
static_assert(traits::has_decibel_scale_v<units::power::dBW<double>>);
```

## Datum traits

Whether a unit carries a **datum** — an arbitrary origin — which makes it a *reading* rather than a magnitude. This
decides which operations are available on it: see [affine temperature](../explain/affine-temperature.md).

| Trait (`_v` form) | Meaning |
|---|---|
| `is_affine_conversion_factor<Cf>` / `is_affine_conversion_factor_v<Cf>` | the conversion factor has a non-zero datum translation. (`is_affine_conversion_factor` in `include/units/core.h`) |
| `is_affine_unit<U>` / `is_affine_unit_v<U>` | `U`'s conversion factor is affine, i.e. `U` is a reading measured from a datum. (`is_affine_unit` in `include/units/core.h`) |
| `is_decibel_level_v<U>` | `U` is a decibel **level**: a *dimensioned* quantity on a logarithmic reference scale (`dBW`, `dBm`). A *dimensionless* decibel value is a **gain** — a relative ratio — and the two obey different rules. |
| `has_arbitrary_origin_v<U>` | `U` is measured from an arbitrary origin — an affine reading **or** a decibel level. Such a value has no origin-free magnitude, sign, remainder, root, power or ratio, so the library refuses those operations on it. |

`has_arbitrary_origin_v` is how generic code should ask. Each refusal is a diagnostic that fires from an overload
*body*, so the overload still resolves and a `requires`-expression reports the operation as available — which means a
`requires`-guarded `if constexpr` hard-errors rather than taking its fallback. Guard on the trait:

```cpp
template<class T>
T scaleIfMeaningful(T value)
{
    if constexpr (!units::traits::has_arbitrary_origin_v<T>)
        return value * 2.0;      // an ordinary quantity, an offset-free scale, a difference, a dB gain
    else
        return value;            // a reading or a level: scaling it has no origin-free meaning
}
```

Three shipped units are affine — `celsius`, `fahrenheit` and `reaumur`. `kelvin` and `rankine` are **not**: they are
absolute scales with no offset, so they behave as ordinary magnitudes. A *difference* of two readings is offset-free,
so it is not affine either.

```cpp
static_assert(traits::is_affine_unit_v<units::temperature::celsius<double>>);
static_assert(traits::is_affine_unit_v<units::temperature::reaumur<double>>);
static_assert(!traits::is_affine_unit_v<units::temperature::kelvin<double>>);
static_assert(!traits::is_affine_unit_v<decltype(units::temperature::celsius<double>(20.0) - units::temperature::celsius<double>(0.0))>);
```

## Accessor traits

Extract member types from a unit or conversion factor, or produce a related type.

| Trait | Provides |
|---|---|
| `unit_traits<T>` | the member typedefs of a `unit`: `numerical_scale_type`, `underlying_type`, `value_type`, `conversion_factor`. (`unit_traits` in `include/units/core.h`) |
| `conversion_factor_traits<T>` | the member typedefs of a `conversion_factor`: `dimension_type`, `conversion_ratio`, `pi_exponent_ratio`, `translation_ratio`. (`conversion_factor_traits` in `include/units/core.h`) |
| `dimension_of_t<U>` | the `dimension_t` of a conversion factor, resolving through nested conversion factors to the SI dimension. (`dimension_of_t` in `include/units/core.h`) |
| `replace_underlying<Unit, U>` / `replace_underlying_t<Unit, U>` | the same unit with its underlying type replaced by `U`. (`replace_underlying` in `include/units/core.h`) |

```cpp
static_assert(std::is_same_v<traits::unit_traits<T>::underlying_type, double>);
static_assert(std::is_same_v<
    traits::conversion_factor_traits<T::conversion_factor>::conversion_ratio, std::ratio<1>>);
static_assert(std::is_same_v<
    traits::dimension_of_t<T::conversion_factor>, units::dimension::length>);
static_assert(std::is_same_v<
    traits::replace_underlying_t<T, float>, units::length::meters<float>>);
```

> **Note:** `dimension_of_t` is a type alias (there is no `dimension_of` predicate); it names the dimension, so compare its result with `std::is_same_v` against a `units::dimension::…` type. `replace_underlying` is SFINAE-friendly — for a non-unit argument it has no `type` member, so `replace_underlying_t` does not participate.

## The `static_assert` pattern and its concept replacement

The historical way to gate a template on a dimension was a `static_assert` inside the body:

```cpp
template <class T>
double magnitude(T x)
{
    static_assert(units::traits::is_area_unit_v<T>, "magnitude(): T must be an area unit");
    return x.value();
}
```

The equivalent constrains the parameter directly with a [concept](concepts.md) and, unlike the `static_assert`, removes the overload from consideration rather than hard-erroring:

```cpp
// prefer a concept where a general one exists (UnitType, DimensionlessUnitType, …):
template <units::UnitType T>
double magnitude(T x) { return x.value(); }
```

There is no dedicated concept for every per-dimension trait; when you need "must be an *area* unit" specifically, the `static_assert(is_area_unit_v<T>, …)` (or a `requires (units::traits::is_area_unit_v<T>)` clause) remains the tool.

## See also

- [Concepts](concepts.md) — the concept wrappers over these predicates, and how to constrain a template.
- [Numerical scales](../explain/scales.md) — `has_linear_scale`, `has_decibel_scale`, and how a scale works.
- [Supported units](supported-units.md) — the dimensions behind the `is_<dimension>_unit` family.
- [Cheat sheet](cheat-sheet.md) — the API on one page.
