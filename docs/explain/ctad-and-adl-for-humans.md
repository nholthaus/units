# CTAD and ADL

*Two C++ features shape how `units` is written. You never invoke them by name, and you do not need to
understand how they are implemented; this page describes what they do and the one or two places where you
have to be deliberate.*

One point from each:

- **CTAD** is why you can write `meters length(5.0)` instead of `meters<double> length(5.0)`.
- **ADL** is why you can write `sqrt(area)` instead of `units::sqrt(area)` (or the old
  `units::math::sqrt(area)`).

The rest of this page is the detail behind those two points.

---

## CTAD: the library fills in the `<double>` for you

CTAD stands for *class template argument deduction*: when you construct a quantity, the compiler looks at
what you pass in and determines the template argument, so you do not write it.

```cpp
meters a(5.0);      // you write this...
meters<double> a(5.0);   // ...and get exactly this
```

Both lines mean the same thing. `meters` on its own is not a "half-written" type — the compiler
completes it from the `5.0` you handed it. This works for construction from a number, from another
quantity, and from a `std::chrono::duration`:

```cpp
meters a(5.0);          // from a number      -> meters<double>
meters b(a);            // from a quantity     -> meters<double>
meters c = 100.0_ft;    // from a literal      -> meters<double>
```

You can always still write the argument explicitly. `meters<double>`, `meters<float>`, and
`meters<int>` are all valid, and you *must* be explicit when there is nothing for the compiler to deduce
from — for example a default-constructed value where you want a specific representation:

```cpp
meters<double> d;   // explicit: there is no argument to deduce from
```

> **Note — CTAD infers `int` vs `double` from what you write.** The deduced representation follows the
> argument's type. This is deliberate and occasionally surprising:
>
> ```cpp
> meters x(5);     // -> meters<int>     (5 is an int)
> meters y(5.0);   // -> meters<double>  (5.0 is a double)
> ```
>
> The unit literals follow the same rule: `5_m` is `meters<int>`, `5.0_m` is `meters<double>`. An
> integer-backed quantity does *integer* arithmetic, so:
>
> ```cpp
> auto p = 1_m / 2_m;      // meters<int> math: p == 0
> auto q = 1.0_m / 2.0_m;  // meters<double> math: q == 0.5
> ```
>
> **When you want fractional results, write the decimal point** (or name the type: `meters<double>`).
> This is the single most common surprise for new users, and it is the same rule the language applies to
> `1 / 2 == 0` for plain `int`.

### The 2.x alias template versus the 3.x class template

In the 2.x line, `meters` was an *alias template* with a default argument, so `meters` (with the angle
brackets and a default) stood in for `meters<double>`, and the singular alias `meter_t` was the common
spelling. In 3.x, `meters` is a *class template* — which is what lets a compiler diagnostic print the
named type `meters<double>` instead of the underlying machinery (see [type safety](type-safety.md)).
CTAD preserves the unadorned spelling across that change: you still write `meters`, and the class-based
type still deduces its argument. The `<>` are optional, the `_t` alias is gone, and the diagnostics name
the type.

---

## ADL: the right function is found without a prefix

ADL stands for *argument-dependent lookup*: when you call a free function, C++ also looks for it in the
namespaces of the arguments' types. Because a quantity's type lives in namespace `units`, an unqualified
call finds the `units` overload:

```cpp
#include <units/length.h>
#include <units/area.h>

units::meters a(3.0), b(4.0);
auto hypotenuse = sqrt(a * a + b * b);   // finds units::sqrt by ADL — no units:: needed
```

You did not write `units::sqrt`. You did not write `using namespace units;` for the function call. The
compiler saw that `a * a + b * b` is a `units` type and looked in `units` for a matching `sqrt`. The
unit-aware overload is found, so the result is a `meters`, not a bare `double`.

This is why the documentation calls math functions plainly — `sqrt`, `hypot`, `sin`, `pow<2>`, `floor` —
with no qualifier. The full set is in [math functions](../how-to/math-functions.md).

### Coming from 2.x: drop the `math::`

In the 2.x line the math wrappers lived in a nested namespace, `units::math`. In 3.x that namespace is
gone; the functions are in `units` and found by ADL. So old code that wrote `units::math::sqrt(x)` should
now write `sqrt(x)` (or `units::sqrt(x)`). See the [migration guide](../meta/migrate-v2-to-v3.md).

> **Caveat — resolving an ambiguous call.** ADL brings the `units` overloads into consideration alongside
> any others in scope. In the rare case that a call is genuinely ambiguous — for example when a same-named
> function is also visible from another namespace — qualify it explicitly with `units::`:
>
> ```cpp
> auto h = units::hypot(a, b);   // force the units overload if an unqualified hypot is ambiguous
> ```

---

## Summary

The two features that shape the syntax:

- Write `meters`, not `meters<double>` — **CTAD** completes it, and it takes `int` or `double` from what
  you pass, so write the decimal point when you want fractions.
- Write `sqrt(x)`, not `units::math::sqrt(x)` — **ADL** finds the unit-aware function because its argument
  is a `units` type.

How the class-based named types are built — the deduction guides, the strong-type registration, the
machinery behind the readable diagnostics — is documented in
[the named-type internals](internals-named-types.md). It is not required to use the library.
