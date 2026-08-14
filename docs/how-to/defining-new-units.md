# Defining new units

Add your own named unit with a single `UNIT_ADD` line that yields a type, its literal, and its printable name.

The library ships 47 dimensions worth of units, but a project frequently needs one more: a domain-specific
length, a custom rate, a unit named for a local convention. You add it declaratively with the `UNIT_ADD`
family of macros, inside `namespace units`. You never hand-write a strong-type class; the macro generates the
type, the user-defined literal, the `name()`/`abbreviation()` metadata used by diagnostics and stream output,
and the trivially-copyable value semantics — from one line.

Related how-to guides: [math functions](math-functions.md), [chrono interop](chrono-interop.md),
[JSON serialization](json-serialization.md).

## The one-line form

`UNIT_ADD` takes four arguments:

```
UNIT_ADD(dimension, plural_name, abbreviation, conversion_factor)
```

| Argument | Meaning | Example |
|---|---|---|
| `dimension` | the dimension namespace the unit joins | `length` |
| `plural_name` | the type name (always plural) | `smoots` |
| `abbreviation` | the literal suffix and printed symbol | `smoot` → `_smoot`, prints `smoot` |
| `conversion_factor` | how the unit relates to an existing one | `conversion_factor<std::ratio<17018, 10000>, meters<>>` |

The macro must be expanded inside `namespace units` so the generated type lands in the right place and the
literal is registered in `units::literals`.

### Example: the smoot

The [smoot](https://en.wikipedia.org/wiki/Smoot) is a length equal to 67 inches (1.7018 m):

```cpp
#include <units/length.h>
#include <iostream>

namespace units
{
    // conversion_factor<ratio, reference-unit>: a smoot is 17018/10000 of a meter.
    UNIT_ADD(length, smoots, smoot, conversion_factor<std::ratio<17018, 10000>, meters<>>)
}

int main()
{
    using namespace units;
    using namespace units::literals;

    auto           bridge    = 364.4_smoot;   // the Harvard Bridge, in smoots
    meters<double> in_meters = bridge;         // converts like any other length

    std::cout << bridge.value() << " smoot = " << in_meters.value() << " m\n";
    // prints: 364.4 smoot = 620.136 m
}
```

The single line gives you the `units::length::smoots` type, the `364.4_smoot` literal, implicit conversion to
and from every other `length` unit, and unit-aware arithmetic — all checked at compile time.

Note: `plural_name` is the type; `abbreviation` is both the literal suffix (`_smoot`) and the symbol printed by
`operator<<` and `name()`. They may differ — `feet` uses abbreviation `ft`, so the literal is `_ft`.

## The conversion factor

The last argument states the unit's magnitude relative to a reference. Two forms cover almost every case.

Scale an existing unit by a `std::ratio`:

```cpp
namespace units
{
    UNIT_ADD(length, spans,  span, conversion_factor<std::ratio<9>,     inches<>>)  // 9 inches
    UNIT_ADD(length, reeds,  reed, conversion_factor<std::ratio<126>,   inches<>>)  // 10.5 feet
    UNIT_ADD(length, points, pt,   conversion_factor<std::ratio<1, 72>, inches<>>)  // 1/72 inch
}
```

Anchor to a dimension's base unit directly (ratio of `1`); this is how each dimension's SI unit is
defined — for example the built-in `meters_per_second` is
`conversion_factor<std::ratio<1>, dimension::velocity>`.

Caveat: a new unit needs both a distinct `plural_name`/`abbreviation` *and* a distinct conversion factor within
its dimension. A duplicate name is a redefinition error; a factor that equals an existing unit's (for example
`std::ratio<6>` of `feet`, which already names `fathoms`) collides in the type-to-name reverse map. Pick a
factor no built-in unit already uses.

Caveat: `std::ratio` is an exact rational. Express an irrational or long-decimal factor as the closest ratio you
need — the smoot's `17018/10000` is exact for 1.7018 m. There is no floating-point conversion-factor argument.

## Adding the metric prefixes

To get the full femto-through-peta family (and their literals) in one line, use
`UNIT_ADD_WITH_METRIC_PREFIXES`. It expands to `UNIT_ADD` for the base unit plus one for each prefix, deriving
each type name and literal from your `plural_name` and `abbreviation`:

```cpp
namespace units
{
    // Generates quaffs/milliquaffs/kiloquaffs/... and _qf/_mqf/_kqf/... literals.
    UNIT_ADD_WITH_METRIC_PREFIXES(length, quaffs, qf, conversion_factor<std::ratio<1, 4>, meters<>>)
}

int main()
{
    using namespace units;
    using namespace units::literals;

    units::length::milliquaffs<double> mq = 3.0_qf;   // 3 qf == 3000 mqf
    std::cout << mq.value() << " mqf\n";               // prints: 3000 mqf
}
```

The prefix names are prepended to `plural_name` (`milliquaffs`, `kiloquaffs`) and the SI prefix letters to
`abbreviation` (`mqf`, `kqf`). This is how the built-in `meters` gets `millimeters`, `kilometers`, and the rest.

For data-style units that also need the binary prefixes (kibi through exbi), use
`UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES`.

## Compound and derived units

A unit built from others — a rate, a product — is spelled with `compound_conversion_factor`, `inverse`,
`squared`, and `cubed`. These accept the underscore-suffixed *conversion-factor* names (`furlongs_`,
`seconds_`), which every `UNIT_ADD` also generates alongside the unit type:

```cpp
#include <units/length.h>
#include <units/velocity.h>
#include <iostream>

namespace units
{
    UNIT_ADD(time,     fortnights,            ftn,         conversion_factor<std::ratio<1209600>, seconds<>>)
    UNIT_ADD(velocity, furlongs_per_fortnight, fur_per_ftn, compound_conversion_factor<furlongs_, inverse<fortnights_>>)
}

int main()
{
    using namespace units;
    using namespace units::literals;

    auto                          v   = 1.0_fur_per_ftn;
    velocity::meters_per_second<double> mps = v;   // 1 fur/ftn == 0.00016631 m/s
    std::cout << mps.value() << " m/s\n";
}
```

`squared<seconds_>` and `cubed<meters_>` express powers; `inverse<hours_>` expresses a reciprocal. The built-in
`feet_per_second_squared` is `compound_conversion_factor<feet_, inverse<squared<seconds_>>>`, and
`meters_per_second` is `compound_conversion_factor<meters_, inverse<seconds_>>` — the same tools you use.

## Defining a whole new dimension

To add a dimension the library does not model, pair your `UNIT_ADD` with `UNIT_ADD_DIMENSION_TRAIT(name)`,
which generates the `traits::is_<name>_unit` / `is_<name>_unit_v` predicates for that dimension. The built-in
headers end with one such line — for example `UNIT_ADD_DIMENSION_TRAIT(length)`.

## Under the hood

You never write a strong-type specialization by hand; that hand-written form was the source of a real defect
(issue #357) and is fully retired. `UNIT_ADD` generates the named-type class, its deduction guides, and the
argument-dependent-lookup machinery that lets diagnostics print the friendly name. If you want the details of
how those named types are built, see the design note on
[named-type internals](../explain/internals-named-types.md).
