# Dimensional analysis

*How the library tracks dimensions through arithmetic, deduces the type of every intermediate result, and
turns a named result type into a compile-time check of your algebra.*

## Dimensions versus units

Two distinct ideas are at work whenever you write a quantity.

A **dimension** is the physical *kind* of a quantity — length, time, mass — independent of how it is
measured. A **unit** is a particular scale for measuring a dimension: meters and feet are two units of
the one dimension *length*; seconds, minutes, and hours are three units of *time*.

`units` encodes both in the type. `meters<double>` and `feet<double>` are different types (different
units) that share the same dimension; that shared dimension is why they convert into one another, and why
you can add them. `meters<double>` and `seconds<double>` have different dimensions, and no amount of
conversion relates them — which is why adding a length to a time is a compile error, not a wrong answer at
run time. The reasoning behind that strictness is in [type safety](type-safety.md) and
[why use a units library](why-units.md).

Dimensional analysis is the bookkeeping that determines, for any expression, what dimension its result
has. The library performs that bookkeeping entirely in the type system, at compile time.

## Multiplication and division compose dimensions

Addition and subtraction leave the dimension unchanged: a length plus a length is a length. Multiplication
and division *compose* dimensions, and the library computes the composite for you.

```cpp
square_meters     area  = 3.0_m * 5.0_m;    // length * length -> area
meters_per_second speed = 90.0_m / 10.0_s;  // length / time   -> velocity
std::cout << area << " | " << speed << '\n';
// 15 m2 | 9 mps
```

The rule is exponent arithmetic on each base dimension. Length has exponent 1; multiplying two lengths
adds the exponents to give length², which is the dimension *area*. Dividing a length by a time gives
length¹·time⁻¹, the dimension *velocity*. Divide again by time and you reach length·time⁻², *acceleration*:

```cpp
auto accel = 30.0_mps / 3.0_s;   // velocity / time -> acceleration
std::cout << accel << '\n';       // 10 mps2  (meters per second squared)
```

Where the composed dimension has a named unit, the library reports the friendly name — `square_meters`
(`m2`), `meters_per_second` (`mps`), `meters_per_second_squared` (`mps2`). Where it does not, the result
is still a perfectly usable compound unit; it simply prints its dimension in exponent form. The mechanism
that recovers the friendly name from a computed result is described in
[the named-type internals](internals-named-types.md).

## The library deduces the result type

You do not annotate the result of an expression; the type system computes it. Each of the results above
has a definite deduced type, which `auto` will accept verbatim:

```cpp
auto a = 3.0_m * 5.0_m;    // square_meters<double>
auto v = 90.0_m / 10.0_s;  // meters_per_second<double>
```

Because the deduction is structural — it follows from the operands' types, by the exponent arithmetic
above — it covers every expression in the program whether or not a test ever runs it, and it is gone by
the time the optimizer runs. Dimensional correctness is a property of the types, and types do not exist at
run time; see [efficiency](efficiency.md).

## Naming the result type checks your algebra

Here is the leverage. When you write `auto`, you accept whatever the expression produces. When you *name*
the result type, you state what you expect — and the compiler verifies that the algebra actually produces
it. A named result type turns dimensional analysis into a check the build performs for you.

```cpp
square_meters<double> area = 3.0_m * 5.0_m;   // asserts: this product is an area. It is. Compiles.
meters<double>        oops = 3.0_m * 5.0_m;   // asserts: this product is a length. It is not. Rejected.
```

The second line does not compile, and the diagnostic names the type you *actually* produced:

```text
error: conversion from 'units::detail::rewrap_to_named_t<units::unit<units::area::square_meters_, double, units::linear_scale> >' {aka 'units::area::square_meters<double>'} to non-scalar type 'units::length::meters<double>' requested
```

That message is the compiler telling you your dimensional analysis is wrong and showing you the right
answer. The remedy is to name the correct type (`square_meters`) or, if the result is genuinely
intermediate, to use `auto`.

## A worked example: F = ma, then kinetic energy

Consider Newton's second law. Force is mass times acceleration, and the SI unit of force, the newton, is
by definition kilogram·meter·second⁻². If you compute `mass * acceleration` and name the result
`newtons`, the compiler confirms the dimensions line up:

```cpp
kilograms                 mass  = 12.0_kg;
meters_per_second_squared accel{ 9.8 };
newtons                   force = mass * accel;   // kg * m/s^2 -> N
std::cout << force << '\n';                        // 117.6 N
```

Name the wrong dimension and the build stops. Asserting that a force is an energy fails to compile, and
the diagnostic names the force type you produced:

```cpp
joules wrong = mass * accel;   // ill-formed: kg * m/s^2 is a force, not an energy
```

```text
error: no matching function for call to 'joules(units::detail::rewrap_to_named_t<units::unit<units::force::newtons_, double, units::linear_scale> >)'
```

The energy actually arrives one step later. Kinetic energy is ½·m·v², and the joule is defined as
kilogram·meter²·second⁻² — exactly what ½·m·v² composes to. Name the result `joules` and, again, the
compiler checks the algebra:

```cpp
kilograms         m = 2.0_kg;
meters_per_second v = 3.0_mps;
joules            energy = 0.5 * m * pow<2>(v);   // kg * (m/s)^2 -> J
std::cout << energy << '\n';                       // 9 J
```

`pow<2>(v)` squares both the value *and* the dimension — velocity² is length²·time⁻² — and multiplying by a
mass yields precisely the dimension of energy. The `0.5` is a dimensionless scalar and does not disturb the
dimension. At no point did you write a conversion factor or spell out the unit of an intermediate; the
type system carried the dimensions through, and naming the final type made it prove the result.

This is the entire proposition of dimensional analysis in the type system: the algebra you would check by
hand on paper is checked by the compiler on every build, with a diagnostic that names the real types when
it does not add up.

---

See also: [type safety](type-safety.md) · [why use a units library](why-units.md) ·
[efficiency](efficiency.md) · [the named-type internals](internals-named-types.md) ·
[the cheat sheet](../reference/cheat-sheet.md)
