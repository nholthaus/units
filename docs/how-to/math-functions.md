# Unit-aware math functions

Call `<cmath>`-style functions on quantities directly — `sqrt(area)`, `hypot(a, b)`, `sin(angle)` — and get
dimensionally correct results.

The library provides unit-aware overloads of the standard math functions. They preserve or transform
dimensions correctly: `sqrt` of an area is a length, `pow<2>` of a length is an area, the trigonometric
functions take an angle and return a dimensionless ratio. You write them exactly as you would the `<cmath>`
originals — unqualified.

Related how-to guides: [defining new units](defining-new-units.md), [chrono interop](chrono-interop.md),
[JSON serialization](json-serialization.md).

## Argument-dependent lookup: write `sqrt`, not `units::math::sqrt`

The math wrappers live in `namespace units`. Because a quantity's type is in `namespace units`, an unqualified
call is found by [argument-dependent lookup (ADL)](https://en.cppreference.com/w/cpp/language/adl): naming
`sqrt(x)` with a units argument selects the unit-aware overload, no qualification required.

```cpp
#include <units/length.h>
#include <units/area.h>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

    meters a = 3.0_m;
    meters b = 4.0_m;

    // pow<2> yields square_meters; sqrt of square_meters yields meters. Both found by ADL.
    meters hypotenuse = sqrt(pow<2>(a) + pow<2>(b));

    std::cout << hypotenuse << '\n';   // prints: 5 m
}
```

Note: the 2.x `units::math` namespace has been **removed**. Code that wrote `units::math::sqrt(x)` or
`math::fma(...)` must drop the `math::` and call the function unqualified (ADL) or, if it must qualify,
`units::sqrt(x)`. There is no `units::math` to resolve against.

## What is wrapped

Every function below is in `namespace units` (except the four classifiers noted separately) and is found by
ADL on a units argument.

| Category | Functions | Dimensional behavior |
|---|---|---|
| Powers and roots | `pow<N>`, `sqrt`, `cbrt` | `pow<N>` raises the dimension to N; `sqrt`/`cbrt` take the root of the dimension |
| Distance / mixing | `hypot` | operands share a dimension; result is that dimension |
| Rounding | `floor`, `ceil`, `round`, `trunc` | dimension preserved |
| Sign and difference | `abs`, `fabs`, `copysign`, `fdim`, `fmod` | dimension preserved (`copysign`/`fdim`/`fmod` on same-dimension operands) |
| Min / max | `min`, `max`, `fmin`, `fmax` | dimension preserved; operands share a dimension |
| Fused multiply-add | `fma` | result dimension is (x·y) combined with z |
| Exponential / logarithmic | `exp`, `exp2`, `expm1`, `log`, `log2`, `log10`, `log1p` | argument and result are **dimensionless** |
| Trigonometric | `sin`, `cos`, `tan` | take an **angle**, return dimensionless |
| Inverse trigonometric | `asin`, `acos`, `atan`, `atan2` | take dimensionless, return **radians** |
| Hyperbolic | `sinh`, `cosh`, `tanh` | take an **angle**, return dimensionless |
| Inverse hyperbolic | `asinh`, `acosh`, `atanh` | take dimensionless, return **radians** |
| Decomposition | `modf` | returns the fractional part; integer part written through the pointer |
| Classification | `isnan`, `isinf`, `isfinite`, `isnormal`, `signbit`, `isunordered` | return `bool` |

## Powers and roots carry dimensions

`pow<N>` is a template on the integer exponent, so the result dimension is known at compile time:

```cpp
using namespace units;
using namespace units::literals;

meters       side = 2.0_m;
square_meters area = pow<2>(side);   // length^2 -> area
cubic_meters  vol = pow<3>(side);    // length^3 -> volume
meters        back = sqrt(area);     // area -> length
```

`hypot(a, b)` computes `sqrt(a^2 + b^2)` on two same-dimension quantities and returns that dimension:

```cpp
meters a = 3.0_m, b = 4.0_m;
meters h = hypot(a, b);   // 5 m
```

## Trigonometry requires an angle

The forward trig and hyperbolic functions accept an angle unit — a plain `double` will not bind. They
return a dimensionless ratio.

```cpp
#include <units/angle.h>

using namespace units;
using namespace units::literals;

dimensionless<double> s = sin(90.0_deg);   // 1.0 — degrees convert to radians internally
dimensionless<double> c = cos(0.0_rad);    // 1.0

// The inverse functions take a dimensionless value and return radians.
angle::radians<double> theta = asin(dimensionless<double>{1.0});   // pi/2 rad
```

Caveat: passing a raw number to `sin`/`cos`/`tan` selects the `<cmath>` overload, not this one, and treats the
value as already-in-radians. Pass an angle quantity (`45.0_deg`, `1.0_rad`) to get the unit-checked behavior.
See the diagnostic in [type safety](../explain/type-safety.md) for the error a bare number produces on some
call sites.

## Logarithms and exponentials are dimensionless

`exp`, `log`, and their relatives operate on and produce dimensionless quantities, matching the mathematics —
the logarithm of a length has no meaning:

```cpp
dimensionless<double> e   = exp(dimensionless<double>{2.0});     // 7.38906
dimensionless<double> ln  = log(dimensionless<double>{100.0});   // 4.60517 (natural log)
dimensionless<double> l10 = log10(dimensionless<double>{100.0}); // 2
```

## Classifiers return `bool`

`isnan`, `isinf`, `isfinite`, and `isnormal` are in `namespace units` and are found by ADL:

```cpp
meters d = 1.0_m;
bool ok = isfinite(d);   // true
bool bad = isnan(d);     // false
```

Caveat: `signbit` (and the `std`-side `isnan`/`isinf`/`isfinite` overloads that exist to win against the
`<cmath>` templates) are declared in `namespace std`, not `namespace units`. `signbit` in particular is **not**
found by ADL on a units argument and is **not** a member of `units`; call it qualified:

```cpp
bool neg = std::signbit(-1.0_m);   // true — note the std:: qualification
```

## Resolving an ambiguous call

Unqualified ADL is the intended way to call these, and normally there is no ambiguity: `std::sqrt` and the like
do not accept a units argument, so even with `using std::sqrt;` in scope the units overload is the only viable
candidate. If a third-party `using`-declaration or another library ever brings a competing overload into scope
and a call does become ambiguous, qualify explicitly with `units::`:

```cpp
meters h = units::sqrt(pow<2>(a) + pow<2>(b));   // force the unit-aware overload
```

This is the same `units::` qualification that resolves any ambiguity elsewhere in the library; reach for it only
when the compiler reports an ambiguous call.
