# Including a subset for faster compiles

*The umbrella header `<units.h>` pulls in every dimension. If a translation unit needs only a few, include
the per-dimension headers instead — you pay compile time only for what you use.*

## The two ways to include

**Everything** — one include, heavier:

```cpp
#include <units.h>   // all 47 dimensions, plus the physical constants
```

**Only what you need** — lighter. Each dimension has its own header under `units/`:

```cpp
#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>   // needed if you name meters_per_second explicitly
```

A per-dimension header is self-contained (it includes the core machinery it needs), so any of them
compiles on its own.

## Including the header your result lands in

Multiplying or dividing quantities produces a new kind: `meters / seconds` is a velocity,
`mass * acceleration` is a force. Each result kind has its own header (`<units/velocity.h>`,
`<units/force.h>`). Including the header the result lands in is a best practice, not a requirement.

With the result's header included:

- The result is the named type: a velocity is `meters_per_second`.
- It prints as `5 mps`.
- Compiler messages about it read `meters_per_second<double>`.

Without it:

- The value and the dimension are correct. Skipping the header does not change the computed number or the
  units.
- The result carries the plain type — a length-over-time — instead of the name `meters_per_second`.
- It prints as `5 m s^-1`, and compiler messages spell out the full `unit<...>` type.
- Code that keys on the exact named type — a `std::hash` or `std::formatter` specialized on
  `meters_per_second`, or an overload taking `meters_per_second` — matches only where the header was
  included.

Skipping a header does not corrupt a value or a dimension. It changes the result's name — its printed form,
its compiler messages, and dispatch that keys on the name.

To make code that dispatches on a result independent of the header set, key it on the dimension
[concept](../reference/concepts.md) rather than the concrete named type. Every dimension has one —
`units::Velocity`, `units::Force`, `units::Length` — and a concept classifies by dimension, so it matches
the same result whether or not the named header was included:

```cpp
// keyed on the named type: matches only where <units/velocity.h> was included
double handle(units::velocity::meters_per_second<double> v);

// keyed on the dimension concept: matches any velocity, in any translation unit
double handle(units::Velocity auto v);
```

For the full account of the type divergence and what stays safe, see
[naming computed results consistently](../explain/naming-computed-results.md).

## Effect on compile time

The library is heavily templated, so translation-unit compile time scales with how much of it you
instantiate. Restricting the includes to the dimensions a file actually uses keeps that file's compile
time down; it does not change run-time behavior or code size (unused templates are never instantiated).
For the broader performance picture, see [efficiency](../explain/efficiency.md).

> **Caveat — the constants live in the umbrella header.** `units::constants` (`c`, `G`, `h`, …) is
> defined in `<units.h>`. If you need a physical constant, include the umbrella header; the per-dimension
> headers do not provide the constants.
