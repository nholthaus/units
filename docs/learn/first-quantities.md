# Creating and inspecting quantities

*A closer look at the four ways to make a quantity, when to name the type versus let it be deduced, and
how to read a value — and a unit — back out.*

This tutorial expands on the quick tour in [getting started](getting-started.md). It assumes you have
included the headers and brought in the namespaces:

```cpp
#include <units.h>
#include <iostream>

using namespace units;             // the quantity types and the ADL math functions
using namespace units::literals;   // the _m, _s, _kg, ... literal operators
```

## The four construction forms

There are four ways to construct a quantity, and they produce the same value. Choose whichever reads
best where you are writing it.

```cpp
meters a(5.0);        // direct-initialization; the type argument is deduced -> meters<double>
meters b = 5.0_m;     // copy-initialization from a unit literal
auto   c = 5.0 * m;   // a scalar times a unit constant (units::m is the meters constant)
meters d{5.0};        // braced (list) initialization
```

Printing all four gives the same quantity:

```cpp
std::cout << a << " | " << b << " | " << c << " | " << d << '\n';
// 5 m | 5 m | 5 m | 5 m
```

`meters` on its own is a complete type: the compiler deduces `meters<double>` from the argument. This is
[class template argument deduction](../explain/ctad-and-adl-for-humans.md). You can always spell the
representation explicitly — `meters<double>`, `meters<float>`, `meters<int>` — when you want a specific
underlying type.

> **Caveat — copy-initialization from a bare `double` is ill-formed.** `meters x = 5.0;` does *not*
> compile: a plain number is not a length, and the conversion is not allowed to happen implicitly (see
> [type safety](../explain/type-safety.md)). Use direct or braced initialization for a literal number —
> `meters x(5.0);` or `meters x{5.0};` — or copy-initialize from something that *is* a quantity:
>
> ```cpp
> meters x = 5.0_m;      // fine: the right-hand side is already a length
> meters y = 100.0_ft;   // fine: feet convert to meters implicitly and exactly
> ```
>
> The one type that *does* copy-initialize from a bare number is a dimensionless quantity, because it has
> no unit to disagree with: `dimensionless<double> r = 0.25;` compiles.

> **Caveat — the decimal point selects `int` vs `double`.** `5.0_m` is `meters<double>`; `5_m` is
> `meters<int>`. This mirrors the language's own rule for numeric literals. It matters because an
> integer-backed quantity does integer arithmetic:
>
> ```cpp
> std::cout << (1_m   / 2_m)   << '\n';   // 0   — integer division truncates
> std::cout << (1.0_m / 2.0_m) << '\n';   // 0.5 — floating-point division
> ```
>
> Write the decimal point whenever you want fractional results.

## Naming the type versus `auto`

When you name the type, the compiler checks that the initializer actually has that type (converting if
the conversion is lossless). When you write `auto`, you accept whatever the expression yields.

```cpp
meters distance = 100.0_ft;   // named: the compiler verifies feet convert to meters, and does it
auto   whatever  = 100.0_ft;  // deduced: whatever is feet<double>
```

For a simple literal this is a matter of taste. It becomes a real safety choice once arithmetic is
involved — see [arithmetic carries dimensions](getting-started.md#arithmetic-carries-dimensions) and
[dimensional analysis](../explain/dimensional-analysis.md). Prefer a named type where a mistake in the
dimensions would be costly; reach for `auto` when the result type is intermediate or verbose.

## Reading the value back out

A quantity does not implicitly convert to a plain number — that would defeat the type safety. When you
must hand a value to an API that does not speak `units`, extract it explicitly. There are three
accessors:

```cpp
meters d = 5.0_m;

double v = d.value();      // 5.0 — the value expressed in the quantity's own units
double r = d.raw();        // 5.0 — the stored value (identical here)
double t = d.to<double>(); // 5.0 — an explicit conversion to a chosen representation
```

- **`.value()`** returns the magnitude in the quantity's units — the number you would read off if you
  wrote the quantity out with its abbreviation.
- **`.raw()`** returns the value as stored. For a linear unit this is identical to `.value()`; for a
  scaled representation such as `percent` or `parts_per_million` the two differ (see
  [scales](../explain/scales.md)).
- **`.to<T>()`** converts the underlying magnitude to the arithmetic type `T` you name — use it when the
  destination type differs from the quantity's representation.

> **Note — `operator()` is gone.** In 2.x you extracted a value by calling the quantity like a function
> (`d()`). That syntax no longer exists in 3.x. Use `.value()`, `.raw()`, or `.to<T>()`.

The one exception to "no implicit number" is a *dimensionless* quantity, which converts to and from
arithmetic types freely, because there is no unit to lose:

```cpp
dimensionless<double> ratio = 0.25;
double back = ratio;   // fine: dimensionless converts implicitly
```

## Printing a quantity

Streaming a quantity writes its value followed by its unit abbreviation:

```cpp
meters d = 5.0_m;
std::cout << d << '\n';               // 5 m
std::cout << (60.0_m / 10.0_s) << '\n'; // 6 mps  (meters per second)
```

`to_string` gives you the same text as a `std::string`:

```cpp
std::string s = to_string(d);   // "5 m"
```

You can also query a unit's name and abbreviation directly:

```cpp
std::cout << d.name()         << '\n';   // meters
std::cout << d.abbreviation() << '\n';   // m

feet f = 3.0_ft;
std::cout << f.name()         << '\n';   // feet
std::cout << f.abbreviation() << '\n';   // ft
```

`name()` returns the plural unit name (`"meters"`, `"feet"`); `abbreviation()` returns the short form
(`"m"`, `"ft"`). Both are `constexpr const char*`.

## Where to next

- [Unit conversions](unit-conversions.md) — turning one unit into another, implicitly and explicitly.
- [Dimensional analysis](../explain/dimensional-analysis.md) — how arithmetic composes dimensions and how
  naming a result type makes the compiler check your algebra.
- [Type safety](../explain/type-safety.md) — the mistakes the library is designed to reject.
- [The cheat sheet](../reference/cheat-sheet.md) — the whole API on one page.
