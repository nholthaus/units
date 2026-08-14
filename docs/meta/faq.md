# FAQ

*Common questions, and the reasoning behind the answers. For a hands-on start see
[getting started](../learn/getting-started.md); for the full API on a page see the
[cheat sheet](../reference/cheat-sheet.md).*

## Why does `1_m / 2_m` give `0`?

Because `1_m` and `2_m` are `meters<int>`, and integer division truncates — exactly as `1 / 2` is `0` for
plain `int`. The representation follows the literal: `5_m` is `meters<int>`, `5.0_m` is `meters<double>`.
Write the decimal point (`1.0_m / 2.0_m == 0.5`) or name a floating-point type when you want fractional
results. See [CTAD and ADL](../explain/ctad-and-adl-for-humans.md).

## What is the difference between `.value()` and `.raw()`?

For ordinary units they return the same number. They differ for *ratio-dimensionless* units such as
`percent`: `.raw()` returns the stored value (the point count) while `.value()` returns the normalized
fraction.

```cpp
auto p = 50.0_pct;
p.raw();     // 50   — the number of points
p.value();   // 0.5  — the fraction
```

Use `.value()` when you want the quantity's value in its own units (the usual case), and `.raw()` when you
specifically want the stored number. Both are distinct from `.to<T>()`, which casts to a chosen
representation. `operator()` — the 2.x accessor — no longer exists; see the
[migration guide](migrate-v2-to-v3.md).

## Why won't a plain `double` convert to a quantity (and vice versa)?

Because a bare number has no dimension, and letting it silently become a length (or a length silently
become a number) is exactly the class of bug the library exists to prevent. Construct explicitly
(`meters d(5.0)`), or extract explicitly (`d.value()`), at the boundary with non-`units` code. The one
exception is a *dimensionless* quantity, which does convert to and from arithmetic types implicitly —
because it genuinely has no dimension.

## Why is a conversion I expected rejected at compile time?

Implicit conversions are allowed only when they are **lossless**. Converting feet to meters into an
`int`-backed quantity would truncate, so it does not happen implicitly:

```cpp
// meters<int> m = 1.0_ft;   // rejected: lossy
meters<double> m = 1.0_ft;   // fine: lossless
int inches = (1.0_ft).to<int>();   // explicit: you asked to round
```

When you intend a narrowing or rounding conversion, ask for it explicitly with `.to<T>()` or
`unit_cast<T>()`. See [type safety](../explain/type-safety.md) and
[unit conversions](../learn/unit-conversions.md).

## Can I use `units` with `std::chrono`?

Yes — time quantities convert implicitly to and from `std::chrono::duration` in both directions, and the
two mix in a single expression via `std::common_type`. See [chrono interop](../how-to/chrono-interop.md).

> **Caveat:** conversion happens at assignment and argument boundaries, not inside a raw mixed expression.
> `1.5_min + std::chrono::seconds{30}` does not compile; assign the chrono value to a units quantity
> first (or vice versa), then combine.

## How do I change the default underlying type from `double`?

Define `UNIT_LIB_DEFAULT_TYPE` before including the library (for example to `float` on an embedded
target). See [configuration](../reference/configuration.md).

## How do I use it without `<iostream>`?

Define `UNIT_LIB_DISABLE_IOSTREAM` (or set the `UNITS_DISABLE_IOSTREAM` CMake option). Arithmetic,
conversions, and `name()`/`abbreviation()` still work; only the stream/string rendering is removed. See
[disabling iostream](../how-to/disabling-iostream.md).

## A literal or unit name collides with a system macro. What do I do?

Some platform headers define macros that clash with the SI abbreviations. `#undef` the offending macro
before including `units`. The known ones: `#undef pascal` on Windows, and several single-letter `#undef`s
(`_U`, `_L`, …) on some ARM toolchains. The Tesla literal is spelled `_Te` rather than `_T` for this
reason (`_T` is a hardcoded Windows macro). See [literals](../reference/literals.md).

## An unqualified math call is ambiguous. How do I fix it?

The unit-aware math functions are found by [ADL](../explain/ctad-and-adl-for-humans.md), which considers
them alongside anything else in scope. If a call is genuinely ambiguous, qualify it: `units::hypot(a, b)`.
Note also that `signbit` is provided as `std::signbit` (to win overload resolution against `<cmath>`), so
call it qualified; `isnan`/`isinf`/`isfinite` are in `units` and work unqualified.

## Does it cost anything at run time?

No. Conversions are compile-time ratio arithmetic, a conversion between equivalent representations emits
no code, and a quantity is a trivially-copyable value the size of its underlying type. See
[efficiency](../explain/efficiency.md).

## What compilers and standard are required?

C++23, on GCC 13+, Clang 19+, or MSVC 2022. The 2.x series is the option for older toolchains. See
[migrating from 2.x](migrate-v2-to-v3.md).

## Is it thread-safe? Does it throw?

Quantities are trivially-copyable value types with no shared state, so passing them by value across
threads is safe (the usual rules apply to a *shared* mutable object, as for any value). The library's
errors are compile-time; the operations do not throw.

## How do I define my own unit?

One line with `UNIT_ADD` inside `namespace units`. See
[defining new units](../how-to/defining-new-units.md).

## How do I cite this library?

Reference the project by name and repository, `nholthaus/units`
(<https://github.com/nholthaus/units>), with the version tag you used.
