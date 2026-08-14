# Getting started

*From `#include` to your first quantities. This assumes a C++23 compiler and that you have the headers
available (see [integration](../how-to/cmake-integration.md) or just put `include/` on your include
path).*

## Include and namespaces

Include the umbrella header for everything, or a single dimension header for a lighter build, and bring
in the literal operators:

```cpp
#include <units.h>                 // everything; or <units/length.h> for just one dimension
#include <iostream>

using namespace units;             // the quantities and the ADL math functions
using namespace units::literals;   // the _m, _s, _kg, ... literal operators
```

`units::literals` must be brought in with a `using` directive to write literals such as `5.0_m`. The
main `units` namespace holds the quantity types and the unit-aware math functions.

## Making a quantity

There are four equivalent ways to create a quantity. Pick whichever reads best in context:

```cpp
meters a(5.0);        // construction; the type argument is deduced (meters<double>)
meters b = 5.0_m;     // a unit literal
auto   c = 5.0 * m;   // a scalar times a unit constant (units::m is the meters constant)
meters d{5.0};        // braced construction
```

`meters` on its own is a complete type — the compiler deduces `meters<double>` from the argument. This is
[CTAD](../explain/ctad-and-adl-for-humans.md); you can also spell it explicitly as `meters<double>` (or
`meters<float>`, `meters<int>`) whenever you want a specific representation.

> **Note — the decimal point selects `int` vs `double`.** `5.0_m` is `meters<double>`; `5_m` is
> `meters<int>`. Integer-backed quantities do integer arithmetic (`1_m / 2_m == 0`), so write the decimal
> point when you want fractional results. This is the same rule the language applies to `1 / 2 == 0`.

## Converting between units

Assigning between compatible units converts implicitly, as long as the conversion is lossless:

```cpp
meters distance = 100.0_ft;   // feet -> meters, implicit and exact
feet   back      = distance;   // and back again
std::cout << distance << " == " << back << '\n';   // 30.48 m == 100 ft
```

A conversion that would lose information (for example into an integer representation) is a compile
error, not a silent truncation — see [type safety](../explain/type-safety.md). Conversions are computed
at compile time and cost nothing at run time; see [efficiency](../explain/efficiency.md).

## Arithmetic carries dimensions

Operators return the correct dimension for the result. Name the result type and the compiler verifies
your dimensional analysis:

```cpp
square_meters     area  = 15.0_m * 5.0_m;    // m * m -> area
meters_per_second speed = 60.0_mi / 1.0_hr;   // a more involved conversion, still implicit
```

If you get the dimension wrong — `meters area = 15.0_m * 5.0_m;` — it does not compile, and the message
names the type you actually produced (`square_meters<double>`). Using `auto` accepts whatever the
expression yields:

```cpp
auto result = 15.0_m * 5.0_m;   // square_meters<double>, whether you expected it or not
```

> **Caveat — `auto` turns off the check.** With an explicit result type, the compiler verifies the
> dimensional analysis. With `auto`, you are asserting that whatever the expression produces is what you
> intended. Prefer an explicit type where a dimensional mistake would be costly; reach for `auto` when the
> result type is genuinely intermediate or verbose.

## Math functions

The unit-aware `<cmath>` functions are found by [ADL](../explain/ctad-and-adl-for-humans.md) — call them
unqualified, with no `units::` prefix:

```cpp
meters a = 3.0_m, b = 4.0_m;
meters hypotenuse = sqrt(pow<2>(a) + pow<2>(b));   // 5 m
```

`pow<N>` and `sqrt` track the dimension (square root of an area is a length); trigonometric functions
require an angle. The full set is in [math functions](../how-to/math-functions.md).

## Getting a plain number back out

When you must hand a value to an API that does not speak `units`, extract it explicitly:

```cpp
meters d = 5.0_m;
double v = d.value();     // 5.0 — the value in the quantity's units
double r = d.raw();       // the stored value (identical here; differs for percent/ppm — see the FAQ)
double t = d.to<double>();// an explicit cast to a chosen representation
```

There is no implicit conversion from a dimensioned quantity to `double` (that would defeat the type
safety); a *dimensionless* quantity is the exception and converts implicitly. `operator()` — the 2.x way
to extract a value — no longer exists; use `.value()`, `.raw()`, or `.to<T>()`.

## Where to next

- [CTAD and ADL, for people who don't like templates](../explain/ctad-and-adl-for-humans.md) — the two
  features behind the terse syntax.
- [The cheat sheet](../reference/cheat-sheet.md) — the whole API on one page.
- [Supported units](../reference/supported-units.md) and [constants](../reference/constants.md).
- [Defining your own units](../how-to/defining-new-units.md).
- Coming from 2.x? [The migration guide](../meta/migrate-v2-to-v3.md).
