# Affine temperature scales: datum translation

Most units in the library relate to their SI base by a pure multiplicative ratio: a foot is `381/1250`
metres, a kilometre is `1000` metres, and converting is a single multiply. Temperature is different.
Celsius and Fahrenheit are **affine** functions of kelvin — they carry a *datum offset* (a zero-point
shift) in addition to a scale factor — so their conversions are not simple ratios and, unlike a pure
ratio, are not a reversible linear transform. The header itself notes this:

> `// NOTE: temperature units have special conversion overloads, since they require translations and`
> `// aren't a reversible transform.` (`temperature.h:53`)

This page explains what the datum offset is, how it appears in a conversion, and the one distinction it
forces on you: an *absolute* temperature versus a temperature *difference*.

## The datum offset in the definitions

A `conversion_factor` has four parameters (the `conversion_factor` template in `include/units/core.h`): a conversion ratio, a base unit/dimension, a
π exponent, and a **translation ratio** — the datum offset. For most units the translation is
`std::ratio<0>`. For the temperature scales it is not (`temperature.h:64`–`68`):

```cpp
UNIT_ADD(temperature, kelvin,     K,    conversion_factor<std::ratio<1>, dimension::temperature>)
UNIT_ADD(temperature, celsius,    degC, conversion_factor<std::ratio<1>,   kelvin_,  std::ratio<0>, std::ratio<27315, 100>>)
UNIT_ADD(temperature, fahrenheit, degF, conversion_factor<std::ratio<5, 9>, celsius_, std::ratio<0>, std::ratio<-160, 9>>)
UNIT_ADD(temperature, reaumur,    Re,   conversion_factor<std::ratio<10, 8>, celsius_>)
UNIT_ADD(temperature, rankine,    Ra,   conversion_factor<std::ratio<5, 9>, kelvin<>>)
```

Reading these:

- **kelvin** is the SI base: ratio `1`, no offset. It is the thermodynamic (absolute) scale.
- **celsius** shares kelvin's *size of a degree* (ratio `1`) but is shifted by the datum
  `27315/100 = 273.15` — the kelvin value of the ice point. So `0 °C` is `273.15 K`.
- **fahrenheit** is defined relative to celsius: a degree Fahrenheit is `5/9` of a degree Celsius, plus
  its own datum offset. Its zero and step differ from both other scales.
- **reaumur** and **rankine** are *pure ratios* of celsius and kelvin respectively — no offset — and so
  behave like ordinary multiplicative units. Rankine is the absolute scale sized in Fahrenheit degrees.

The presence of a non-zero translation ratio is exactly what makes a scale affine rather than linear.

## An absolute conversion includes the offset

When you convert an *absolute* temperature, the datum offset participates. The library's `convert`
function detects a non-zero translation and adds it after applying the ratio (the translation branches of `convert` in `include/units/core.h`).
The numbers below are the real output of the program (compile and run to confirm):

```cpp
#include <units/temperature.h>
#include <iostream>

int main()
{
    units::temperature::celsius<double> boiling(100.0);

    units::temperature::kelvin<double>     k(boiling);   // 373.15 K   (100 + 273.15)
    units::temperature::fahrenheit<double> f(boiling);   // 212 F

    std::cout << boiling.value() << " C = "
              << k.value() << " K = "
              << f.value() << " F\n";                    // 100 C = 373.15 K = 212 F

    units::temperature::fahrenheit<double> freezing(units::temperature::celsius<double>(0.0));
    std::cout << "0 C = " << freezing.value() << " F\n"; // 0 C = 32 F  (the offset, not 0)
    return 0;
}
```

Note that `0 °C` converts to `32 °F`, not `0 °F`: the offset is what carries the ice point across. A
pure-ratio unit would send `0` to `0`; an affine unit does not.

## Absolute temperature versus temperature difference

This is the one caveat the datum offset forces, and it is worth stating precisely.

> **Caveat (absolute vs difference):** a *temperature* and a *temperature interval* are different
> quantities that happen to share a unit name. `20 °C` is an absolute point on the Celsius scale; a `1 °C`
> *rise* is an interval. Converting an absolute point applies the datum offset (`20 °C → 293.15 K`);
> converting an interval does **not** (`a 1 °C step is a 1 K step`, but a `1 °F` step is a `5/9 K` step).
> The unit types in this library model **absolute temperatures** — every stored value is a point on the
> scale — so their conversions always carry the offset.

A concrete illustration of why absolute and difference cannot be the same operation: the Celsius and
Fahrenheit scales cross at `−40`, where a single number reads the same on both scales even though the
scales are genuinely different. This is a property of the *affine* mapping, not of any interval:

```cpp
#include <units/temperature.h>
#include <iostream>

int main()
{
    units::temperature::celsius<double>    c(-40.0);
    units::temperature::fahrenheit<double> f(c);
    std::cout << "-40 C = " << f.value() << " F\n";   // -40 C = -40 F
    return 0;
}
```

Because the type stores an absolute point, the practical rule is: **do not reach for an affine
temperature type to represent a difference.** If you need to add or scale temperature *intervals* freely,
work in the absolute (offset-free) scales — kelvin or rankine — where a value and an interval coincide
numerically, or keep intervals as plain dimensionless factors and apply them explicitly. Rankine's
purely multiplicative definition (no offset) is why `1 K` maps cleanly to `1.8 Ra`:

```cpp
#include <units/temperature.h>
#include <iostream>

int main()
{
    units::temperature::rankine<double> ra(units::temperature::kelvin<double>(1.0));
    std::cout << "1 K = " << ra.value() << " Ra\n";   // 1 K = 1.8 Ra  (pure 9/5 ratio, no offset)
    return 0;
}
```

## Why "not a reversible linear transform"

A linear transform `x ↦ a·x` composes and inverts by multiplying and dividing ratios — the machinery the
unit manipulators (`inverse`, `squared`, `sqrt`, compound units) rely on. An affine transform
`x ↦ a·x + b` does not: it has an additive term that a multiply cannot express, and the manipulators
deliberately *drop* the translation ratio when they combine units (see `inverse_impl` in `include/units/core.h`,
which comments that "inverses are rates or changes, so translation factor is removed"). That is why the
offset lives only in the direct scale-to-scale conversion path and not in derived-unit algebra — and why
the header flags temperature conversions as special, non-reversible transforms rather than ordinary
ratio conversions.

## See also

- [Numerical scales: linear and decibel](scales.md) — a different non-ratio behavior (logarithmic scale).
- [Supported units](../reference/supported-units.md) — the full temperature table (kelvin, celsius,
  fahrenheit, reaumur, rankine).
- [Defining new units](../how-to/defining-new-units.md) — the `conversion_factor` translation parameter.
- [Type safety and dimensional analysis](type-safety.md) — why an absolute temperature is still a
  temperature-dimensioned quantity.
