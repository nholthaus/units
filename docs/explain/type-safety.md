# Type safety: the mistakes `units` is designed to reject

*The reason to give a quantity a type is that the compiler then refuses the operations that make no
physical sense. This page catalogs the mistakes the library is built to catch, and shows the **real,
verbatim** diagnostic each one produces. The diagnostics below are captured directly from the compiler by
the [error-message test harness](../../test/errorMessages/) (`run.py --emit-doc`), so what you read here
is what you will actually see — never a paraphrase.*

Each example is a case in `test/errorMessages/cases/`; the harness asserts that it fails to compile
**and** that the diagnostic names the friendly unit type. The text shown is GCC 13. Clang and MSVC
produce equivalent messages (different wording, same named types); the harness verifies all three.

A recurring theme: 3.x names the *friendly* type — `meters<double>`, `seconds<double>`,
`square_meters<double>` — instead of the underlying `conversion_factor<...>` template. That readability
is the point of the class-based named units; how it is built is covered in
[the named-type internals](internals-named-types.md).

---

## Adding incompatible dimensions

You cannot add a length to a time. The diagnostic names both operands.

```cpp
auto bad = 1.0_m + 1.0_s;   // length + time
```

```text
readable_add_incompatible.cpp:9:18: error: no match for ‘operator+’ (operand types are ‘units::length::meters<double>’ and ‘units::time::seconds<double>’)
    9 | auto bad = 1.0_m + 1.0_s; // ill-formed: cannot add length and time
      |            ~~~~~ ^ ~~~~~
      |            |       |
      |            |       units::time::seconds<double>
      |            units::length::meters<double>
```

---

## Assigning a result of the wrong dimension

Multiplying two lengths produces an *area*. Assigning that to a `meters` is rejected. GCC surfaces the
product through an internal alias but names the friendly type right beside it in `{aka …}`:

```cpp
units::length::meters<double> a = 1.0_m * 1.0_m;   // m * m is an area, not a length
```

```text
readable_wrong_result_type.cpp:10:41: error: conversion from ‘units::detail::rewrap_to_named_t<units::unit<units::area::square_meters_, double, units::linear_scale> >’ {aka ‘units::area::square_meters<double>’} to non-scalar type ‘units::length::meters<double>’ requested
   10 | units::length::meters<double> a = 1.0_m * 1.0_m; // ill-formed: m*m is an area, not a length
      |                                   ~~~~~~^~~~~~~
```

The remedy is to name the correct result type — `square_meters a = 1.0_m * 1.0_m;` — or use `auto`.

---

## Narrowing into an integer representation

A conversion that would lose information does not happen implicitly. Feet-to-meters is not an
integer-exact ratio, so it cannot bind to a `meters<int>`:

```cpp
units::length::meters<int> a = 1.0_ft;   // lossy: would truncate
```

```text
readable_narrowing_to_int.cpp:10:32: error: conversion from ‘units::length::feet<double>’ to non-scalar type ‘units::length::meters<int>’ requested
   10 | units::length::meters<int> a = 1.0_ft; // ill-formed: narrowing/lossy into an integer underlying
      |                                ^~~~~~
```

Implicit conversions are allowed only when they are lossless. See
[the FAQ](../meta/faq.md) on integer representations, and use a floating-point representation
(`meters<double>`) or an explicit `.to<int>()` (which truncates toward zero) when you intend to narrow.

---

## Mixing a bare scalar with a dimensioned quantity

A raw `double` is not a length; you cannot add one to a `meters`. (A *dimensionless* quantity is the one
exception — it converts to and from arithmetic types implicitly.)

```cpp
auto bad = 1.0_m + 5.0;   // a length plus a plain number
```

```text
readable_scalar_plus_unit.cpp:8:18: error: no match for ‘operator+’ (operand types are ‘units::length::meters<double>’ and ‘double’)
    8 | auto bad = 1.0_m + 5.0; // ill-formed: cannot add a raw scalar to a length
      |            ~~~~~ ^ ~~~
      |            |       |
      |            |       double
      |            units::length::meters<double>
```

---

## Passing the wrong dimension to a dimensional function

Trigonometric functions require an *angle*. Calling `sin` on a length is rejected. (Note that `sqrt` of a
length is **not** an error — `units` supports rational dimensions, so the square root of a length is a
well-defined type. It is trigonometry, logarithms, and the like that require a dimensionless or angular
argument.)

```cpp
auto bad = sin(1.0_m);   // sin needs an angle, not a length
```

```text
readable_trig_needs_angle.cpp:8:16: error: cannot convert ‘units::length::meters<double>’ to ‘double’
    8 | auto bad = sin(1.0_m); // ill-formed: sin expects an angle, not a length
      |                ^~~~~
      |                |
      |                units::length::meters<double>
```

---

## Comparing across dimensions

Two quantities of different dimensions are not ordered, so a relational comparison between a length and a
mass does not compile. The failure surfaces in `std::common_type` (there is no common type for the two),
and it names both units:

```cpp
bool bad = (1.0_m < 1.0_kg);   // incomparable dimensions
```

```text
/usr/include/c++/15/type_traits: In substitution of ‘template<class ... _Tp> using std::common_type_t = typename std::common_type::type [with _Tp = {units::unit<units::length::meters_, double, units::linear_scale>, units::unit<units::mass::kilograms_, double, units::linear_scale>}]’:
core.h: required from ‘constexpr bool units::unit<...>::operator<(...) const [with ... ConversionFactor = units::length::meters_; ... ConversionFactorRhs = units::mass::kilograms_ ...]’
```

The template context is longer than the others because the comparison is rejected through
`std::common_type`; the useful part is the substitution line, which names `meters_` and `kilograms_` as
the two types with no common type.

---

## Why this is a feature, not a nuisance

Every diagnostic above corresponds to a real defect a bare `double` would have accepted silently — a unit
mismatch, a dimensional error, a lossy conversion, a category confusion. Catching them at compile time,
with a message that names the actual types, is the entire value proposition. For the design reasoning
behind the strictness (why conversions are lossless-only, why scalars and dimensioned quantities do not
mix), see [why units](why-units.md) and [the FAQ](../meta/faq.md).
