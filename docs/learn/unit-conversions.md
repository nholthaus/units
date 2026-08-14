# Unit conversions

*How one unit becomes another: implicit and lossless where the compiler can prove it safe, explicit where
you are asking for something narrowing or deliberate.*

This tutorial assumes the setup from [getting started](getting-started.md):

```cpp
#include <units.h>
#include <iostream>

using namespace units;
using namespace units::literals;
```

## Implicit conversion between compatible units

Assigning or constructing one unit from another of the *same dimension* converts automatically. The
library holds a single authoritative conversion factor for each unit, so the arithmetic is applied in one
place — you never re-type it at the call site.

```cpp
meters distance = 100.0_ft;   // feet -> meters, implicit and exact
feet   back     = distance;   // and back again
std::cout << distance << " == " << back << '\n';
// 30.48 m == 100 ft
```

Construction converts the same way:

```cpp
meters m1{ 3.0_ft };   // 0.9144 m
```

Two properties make this safe to rely on:

- **It is computed at compile time.** A unit is defined as a `std::ratio` relative to its dimension's base
  unit, so the factor between any two compatible units is a compile-time constant. At run time a
  conversion is a single multiply — or nothing at all, when the source and destination denote the same
  type. See [efficiency](../explain/efficiency.md) for the full argument.
- **It happens only when it is lossless.** The compiler permits the implicit conversion exactly when it
  can prove no information is lost.

## Lossless-only: a lossy conversion is a compile error

An implicit conversion that could lose information does not happen silently — it is rejected at compile
time. The canonical case is converting into an integer representation whose ratio is not exact:

```cpp
meters<int> a = 1.0_ft;   // ill-formed: 1 ft is 0.3048 m, not representable exactly as an int
```

The compiler refuses this rather than truncating behind your back:

```text
error: conversion from 'units::length::feet<double>' to non-scalar type 'units::length::meters<int>' requested
```

The diagnostic names the types on both sides. The remedy depends on what you actually meant:

- If you wanted the exact value, use a floating-point representation: `meters<double> a = 1.0_ft;`.
- If you genuinely want to round or truncate into an integer, say so explicitly — see the next section.

This is the general rule, not a special case for integers: any conversion the compiler cannot prove
lossless requires you to ask for it explicitly. For the full catalog of what the type system rejects and
the verbatim diagnostics, see [type safety](../explain/type-safety.md).

## Converting through arithmetic

Conversions also fall out of arithmetic. When you divide a distance by a time, the operands may be in any
compatible units; the result carries the correct dimension and the factors are composed for you:

```cpp
meters_per_second speed = 60.0_mi / 1.0_hr;   // miles and hours in, meters per second out
std::cout << speed << '\n';                   // 26.8224 mps
```

You named the result `meters_per_second`, so the compiler verified the algebra and applied every
conversion factor — miles to meters, hours to seconds — as a single compile-time constant. How the result
dimension is deduced is the subject of [dimensional analysis](../explain/dimensional-analysis.md).

## Deliberate and narrowing conversions

When you want a conversion the compiler will not do implicitly — because it is lossy, or because you need
a plain number at an interface boundary — reach for an explicit extractor.

**`.to<T>()`** returns the magnitude, expressed in the quantity's own units, as the arithmetic type `T`:

```cpp
feet f = 30.48_m;         // 100 ft, held as feet<double>
int  n = f.to<int>();     // 100 — the value in feet, converted to int
```

**`unit_cast<T>(...)`** does the same thing in free-function form; it strips the unit and returns a bare
arithmetic value, for use at the boundary with code that does not use `units`:

```cpp
int n = unit_cast<int>(f);   // 100
```

For a deliberate *lossy conversion between two units* — say, feet to whole meters — do the lossless part
implicitly, then round explicitly into the integer type. The truncation is now visible in the source, at
the one place you intended it:

```cpp
feet        height = 71.0_ft;
meters      exact  = height;                          // implicit, lossless: 21.6408 m
meters<int> rounded{ static_cast<int>(exact.value() + 0.5) };
std::cout << exact << " -> " << rounded << '\n';
// 21.6408 m -> 22 m
```

> **Note — extraction is a boundary.** Keep quantities typed for as long as they stay inside your code;
> convert to a plain number only where you must hand a value to an API, a serialization format, or a
> hardware register. Concentrating the extraction at the boundary keeps the loss of type information
> explicit and local.

## Where to next

- [Dimensional analysis](../explain/dimensional-analysis.md) — how multiplication and division compose
  dimensions and deduce result types.
- [Type safety](../explain/type-safety.md) — the mistakes the compiler rejects, with verbatim diagnostics.
- [Efficiency](../explain/efficiency.md) — why every conversion above has no run-time cost.
- [The cheat sheet](../reference/cheat-sheet.md) — the API on one page.
