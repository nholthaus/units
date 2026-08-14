# No run-time cost

*The safety `units` provides lives in the type system. Types do not exist at run time, so neither does
the safety's cost: an optimized build produces the same machine code as the equivalent hand-written
arithmetic on plain numbers.*

Dimensional correctness has no run-time cost. Every check happens during compilation; every conversion
factor is known to the compiler; every quantity is a trivial value the size of the number it holds. This
page explains why, with examples you can compile and inspect.

## Conversions are compile-time ratio arithmetic

A unit is defined as a `std::ratio` relative to its dimension's base unit. `feet` is
`std::ratio<381, 1250>` of a meter; `inches` is `std::ratio<1, 12>` of a foot; and so on. Because those
ratios are types, the conversion factor between any two units of the same dimension is computed by the
type system as a compile-time constant. Converting a quantity multiplies its stored value by that
constant.

```cpp
#include <units.h>

units::feet<double> f = units::meters<double>{1.0};   // f.value() == 3.28083...
```

The `381/1250` (and its reciprocal for the other direction) is resolved before code generation. At run
time this is a single floating-point multiply — the same instruction you would have written by hand had
you remembered the factor and typed it correctly.

## A recursively-defined conversion collapses to one operation

Units are defined in chains. In `units/time.h`:

```
seconds = base
minutes = 60 seconds
hours   = 60 minutes
days    = 24 hours
weeks   =  7 days
years   = 365 days
```

Nothing in that chain defines "years to weeks" directly. If the chain were followed at run time, a
`years` → `weeks` conversion would walk `years → days → weeks`, and a `years` → `seconds` conversion
would walk five links. It is not followed at run time. `std::ratio` multiplication composes the whole
chain into a single reduced ratio at compile time:

```cpp
#include <units.h>
using namespace units::literals;

constexpr units::weeks<double>   w = units::years<double>{1.0};
static_assert(w.value() == 365.0 / 7.0);            // one ratio, resolved at compile time

constexpr units::seconds<double> s = units::years<double>{1.0};
static_assert(s.value() == 365.0 * 24.0 * 60.0 * 60.0);   // == 31'536'000
```

Both `static_assert`s pass, which is proof that the results are available as compile-time constants. The
five-link `years → seconds` conversion emits, at most, one multiply — and when the source value is
itself a constant, as above, it emits nothing at all: the answer is baked in.

## Conversions between equivalent representations emit no code

When the source and destination are the same type, there is nothing to convert. The library does not
insert a multiply-by-one; the assignment is an ordinary copy of the underlying value.

```cpp
#include <units.h>

units::meters<double> a{5.0};
units::meters<double> b = a;    // no conversion arithmetic — a copy of the double
```

The same holds when two type spellings denote the identical type. `meters`, `meters<>`, and
`meters<double>` name one type; assigning between them is a copy, never a conversion. (See
[the namespace map](namespaces.md) and [CTAD and ADL](ctad-and-adl-for-humans.md) for why those
spellings coincide.)

## Quantities are trivial value types

A quantity holds exactly one number. It adds no vtable, no tag, no bookkeeping field — the unit is
encoded in the type, which occupies no storage. The standard traits confirm this:

```cpp
#include <units.h>
#include <type_traits>

static_assert(std::is_trivially_copyable_v<units::meters<double>>);
static_assert(sizeof(units::meters<double>) == sizeof(double));
```

Trivial copyability is a load-bearing property, deliberately preserved by the named-unit types: it means
a quantity can be `memcpy`'d, passed in a register, and placed in `constexpr` and trivially-relocatable
contexts exactly as its underlying type can. `meters<int>` is trivially copyable and the size of an
`int`; `meters<float>` the size of a `float`; and so on.

## A worked compile-time computation

Because conversions and the algebraic operations are `constexpr`, an entire calculation can be evaluated
by the compiler and checked with a `static_assert` — which simultaneously demonstrates that no run-time
work remains, since a value the compiler can assert on is a value it has already computed:

```cpp
#include <units.h>

// average speed over a 402 m sprint in 9.58 s
constexpr units::meters_per_second<double> v =
    units::meters<double>{402.0} / units::seconds<double>{9.58};

static_assert(v.value() > 41.0 && v.value() < 42.0);   // computed at compile time
```

The division of a length by a time produces the correct velocity *type* by type-system arithmetic, and
the numeric result is a compile-time constant. There is no run-time division, no unit lookup, and no
dispatch.

## What the assembly looks like

For an optimized build, the code generated for typed-quantity arithmetic matches the code generated for
the corresponding plain-`double` arithmetic. The disassembly below is `-O2` unless noted; GCC 15 and
Clang 21 agree.

**A runtime expression.** Compute a distance from a speed in mph and a time in seconds — the raw version
hard-codes the mph → m/s factor, the `units` version carries it in the types:

```cpp
double         distance_raw  (double mph, double sec)                        { return (mph * 1609.344 / 3600.0) * sec; }
meters<double> distance_units(miles_per_hour<double> v, seconds<double> t)   { return v * t; }
```

Both are three floating-point instructions — a multiply, a divide, a multiply — differing only in
operand order (GCC 15):

```asm
distance_raw:                        distance_units:
    mulsd   .LC0(%rip), %xmm0            mulsd   %xmm1, %xmm0
    divsd   .LC1(%rip), %xmm0            mulsd   .LC2(%rip), %xmm0
    mulsd   %xmm1, %xmm0                 divsd   .LC3(%rip), %xmm0
    ret                                  ret
```

**A same-unit conversion is nothing.** Passing a `meters` where a `meters` is wanted is not a cheap
conversion — the function is a single `ret`:

```cpp
double roundtrip(meters<double> m) { meters<double> copy = m; return copy.value(); }
```
```asm
roundtrip:
    ret
```

**A compile-time conversion is done by the compiler.** A conversion of known values folds to a single
constant load; the arithmetic never runs:

```cpp
double speed_limit_mps() { return meters_per_second<double>(65.0_mph).value(); }
```
```asm
speed_limit_mps:
    movsd   .LC0(%rip), %xmm0        ; xmm0 = 29.0576  (65 mph, converted at compile time)
    ret
```

**A hot loop vectorizes identically.** Summing an array of `kilometers` as `meters` produces the same
instruction stream — including the AVX vectorization at `-O3 -march=x86-64-v3` — as the equivalent
raw-`double` loop.

In each case the `units` version carries no wrapper, no extra load, and no branch: the type is gone, and
only the arithmetic remains.

## The `constexpr` caveat

Most of the API is usable in a constant expression: construction, conversion, comparison, the arithmetic
operators, and the *algebraic* math functions `sqrt` and `pow`, which are implemented with the library's
own compile-time rational routines.

Two areas are the exception, and for the same underlying reason — they call standard-library functions
that are not themselves `constexpr`:

- **The `<cmath>`-backed wrappers** — the trigonometric functions (`sin`, `cos`, `tan`, and inverses),
  `exp`, `log`, and similar — forward to `std::sin`, `std::cos`, and so on. Those are not `constexpr`, so
  neither are the wrappers.
- **The decibel scale.** Its conversion between logarithmic and linear representation is a run-time
  computation, not a compile-time ratio.

Concretely:

```cpp
#include <units.h>

constexpr auto ok  = sqrt(units::square_meters<double>{16.0});   // fine: algebraic, constexpr
// constexpr auto no = cos(units::radians<double>{0.0});         // error: cos is not constexpr
```

The first line compiles; the second does not, because `cos` cannot run at compile time. This is a
property of the standard functions being wrapped, not of `units`. At run time both are ordinary,
optimizable calls — the caveat is strictly about compile-time evaluation.

> **Caveat:** "not `constexpr`" means "not usable in a constant expression such as a `static_assert` or a
> template argument." It does *not* mean slow. The trigonometric wrappers compile to a single call to the
> standard-library function, with the angle already converted to radians by a compile-time ratio. Their
> run-time cost is exactly that of calling `std::sin` yourself.

---

See also: [why use a units library](why-units.md) · [the namespace map](namespaces.md) ·
[CTAD and ADL](ctad-and-adl-for-humans.md) · [scales](scales.md) ·
[the cheat sheet](../reference/cheat-sheet.md)
