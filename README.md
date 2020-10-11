# UNITS
<a id="markdown-units" name="units"></a>

A compile-time, header-only, dimensional analysis library built on C++17 with no dependencies.

[![Linux build](https://travis-ci.org/nholthaus/units.svg?branch=master)](https://travis-ci.org/nholthaus/units) [![Windows build](https://ci.appveyor.com/api/projects/status/github/nholthaus/units?svg=true&branch=master)](https://ci.appveyor.com/project/nholthaus/units) [![Coverage Status](https://coveralls.io/repos/github/nholthaus/units/badge.svg?branch=master)](https://coveralls.io/github/nholthaus/units?branch=master) ![license](https://img.shields.io/badge/license-MIT-orange.svg) ![copyright](https://img.shields.io/badge/%C2%A9-Nic_Holthaus-orange.svg) ![language](https://img.shields.io/badge/language-c++-blue.svg) ![c++](https://img.shields.io/badge/std-c++14-blue.svg)<br>![msvc2015](https://img.shields.io/badge/MSVC-2015-ff69b4.svg) ![msvc2017](https://img.shields.io/badge/MSVC-2017-ff69b4.svg) ![gcc-4.9.3](https://img.shields.io/badge/GCC-4.9.3-ff69b4.svg) ![gcc-5.4.0](https://img.shields.io/badge/GCC-5.4.0-ff69b4.svg) ![clang-3.4](https://img.shields.io/badge/CLANG-3.4-ff69b4.svg)

# Get in touch
<a id="markdown-get-in-touch" name="get-in-touch"></a>

If you are using `units.h` in production code, I'd love to hear from you via GitHub issues!

# Next Release - v3.0.0

## New in v3.0.0

> I got enamoured with this library because with a few lines
I could have a powerful type representing my domain-specific concept.
Being able to clearly express your domain in code leads to better designs.
My `Pixels` unit is as well-supported as any other unit in this library.
It has access to the expressiveness and dimensional analysis
that the library-defined units have.
It's amazing how this library lets me define this expressive domain unit
so succinctly and have access to so many useful things
in such a short amount of code. \
-- Johel Ernesto Guerrero Peña (@johelegp),
https://github.com/nholthaus/units/issues/124#issuecomment-390773279

# Latest Release - v2.3.1
<a id="markdown-latest-release---v2.3.1" name="latest-release---v2.3.1"></a>

## Get it
<a id="markdown-get-it" name="get-it"></a>

[![DOWNLOAD](https://img.shields.io/badge/Download-v2.3.1-green.svg)](https://github.com/nholthaus/units/releases/tag/v2.3.1)

## New in v2.3.1
<a id="markdown-new-in-v2.3.1" name="new-in-v2.3.1"></a>

**This version removes support for the Visual Studio 2013 compiler.**

Features:
- units now include constexpr `name()` and `abbreviation()` member functions, which do not really on string/iostream.
- Builds with VS2017 Ninja generator out of the box
- string conversions are now locale aware
- added unary incremement and decremement operators (`++`,`--`), as well as unary `+` operator.

Bug fixs:
- fixed compilation error when iostream was disabled

## New in v2.3.0
<a id="markdown-new-in-v2.3.0" name="new-in-v2.3.0"></a>

Features:
- 5x compile time improvement on MSVC.
- 1.5x compile time improvement on GCC.
- Even more dramatic reductions in compile time can be acheived if you opt-in to specific unit definitions instead of using all the library-defined types (which is the default value). Check out [Enabling a subset of units to improve compilation time](#enabling-a-subset-of-units-to-improve-compilation-time) for instructions.
- Adds std::cout support for units with no defined abbreviation (they show up as a combination of SI base units)
- Support for `std::numeric_limits` of unit types.
- Assignment operators for unit types: `-=`, `+=`, `/=`, `*=`.
- Added `min` and `max` overloads for units types in `units::math`.
- Added `to_string` function and `abbreviation` functions:
  ```cpp
  auto len = 3.5_m;
  auto str = units::length::to_string(len);
  auto abv = units::length::abbreviation(len);

  std::cout << str;  // prints "3.5 m"
  std::cout << abv;  // prints "m"
  ```
- Added units of data and data transfer: `bits`, `bytes`, `bits_per_second`, and `bytes_per_second`.
- Adds `value()` member for accessing underlying type.
- Adds `value_type` trait, as a synonym for `underlying_type`.
- Adds definitions for Julian and Gregorian years.
- Thanks to @dinocore1, `units` now supports cmake install and `find_packages`. From the [pull request](https://github.com/nholthaus/units/pull/84):
  ```cmake
  # To have cmake install units library to a local 'install' directory:
  mkdir build
  cd build
  cmake -DCMAKE_INSTALL_PREFIX="install" ..
  cmake --build . --target install
  # The units library can then be used in some other cmake project using
  # the standard 'find_package' command. Like so:
  find_package(units)
  ```
Bug fixes:
- Fixed singualr name of `siemen` to be `siemens` (Thanks @Oxyd)
- Fixed bug with `cubrt` operation (Thanks @PearCoding)
- fixed constexpr relational operators bug
- fixed exponential temperature conversions (Thanks @guarndt)

## Tested on
<a id="markdown-tested-on" name="tested-on"></a>

 - gcc-7
 - clang-5
 - msvc2017

Does this library work on your compiler? If so, let me know!

# Contents
<a id="markdown-contents" name="contents"></a>

<!-- TOC -->

- [UNITS](#units)
- [Get in touch](#get-in-touch)
- [Latest Release - v2.3.1](#latest-release---v231)
	- [Get it](#get-it)
	- [New in v2.3.1](#new-in-v231)
	- [New in v2.3.0](#new-in-v230)
	- [Tested on](#tested-on)
- [Contents](#contents)
- [Documentation](#documentation)
- [Description](#description)
- [Getting started guide](#getting-started-guide)
- [Unit initialization](#unit-initialization)
- [Conversion factors](#conversion-factors)
- [Unit types](#unit-types)
- [Unit Literals](#unit-literals)
- [<cmath> Functions](#cmath-functions)
- [Exponentials and Square Roots](#exponentials-and-square-roots)
- [Removing type safety](#removing-type-safety)
- [Efficiency](#efficiency)
- [constexpr support](#constexpr-support)
- [Conversion without unit types](#conversion-without-unit-types)
- [Namespaces](#namespaces)
- [Defining new units](#defining-new-units)
- [Unit definition macros](#unit-definition-macros)
- [Unit Type Traits](#unit-type-traits)
- [Underlying type of units](#underlying-type-of-units)
- [Disabling IOStream](#disabling-iostream)
- [Improve compilation time](#improve-compilation-time)
- [MSVC compile options](#msvc-compile-options)
	- [Per-Dimension Headers](#per-dimension-headers)
- [Macro clashes](#macro-clashes)
	- [Windows macros](#windows-macros)
	- [ARM macros](#arm-macros)
- [CMake Instructions](#cmake-instructions)
- [Build Instructions](#build-instructions)
	- [Windows](#windows)
	- [Linux](#linux)
- [Previous Releases](#previous-releases)

<!-- /TOC -->

# Documentation
<a id="markdown-documentation" name="documentation"></a>

[The full documentation is available ***here***](http://nholthaus.github.io/units).

# Description
<a id="markdown-description" name="description"></a>

The library consists of an all-including header ([<units.h>](include/units.h)), per-dimension headers (of the form [<units/dimension.h>](include/units/)), plus unit tests. To incorporate the library into your project, simply copy the [include](include) directory into your include path, or add the [included CMake project](#cmake-instructions) into your build. Using the CMake project, you can also build the unit tests and documentation if desired.

The library provides a set of conversion factors, unit types, and traits to solve dimensional analysis problems, that is, problems involving dimensioned physical quantities. The conversions between units are defined as ratios at compile time, making the library _incredibly_ fast. Additionally, specifying units as _types_, rather than variable name suffixes (or not at all), provides complete type-safety within the compiler. This means that code that accidentally misuses units or which has errors in the dimensional analysis _will fail at compile-time, not at run-time_.

The unit test file `unitTests/main.cpp` contains example usage of every type, trait, and function contained in the library, and while not exactly user-friendly, can be a valuable resource.

# Getting started guide
<a id="markdown-getting-started-guide" name="getting-started-guide"></a>

Include `<units.h>` in your sources, along with the `using` directive for literals

```cpp
#include <units.h>

using namespace units::literals;
```

Each "dimension" of unit is defined in its own inline namespace. See [the namespaces section](#namespaces) for a list. The rest of the guide assumes you're `using namespace units;`.

**The easiest way to get started with the `units` library is to think of unit types as arithmetic value wrappers.** Unit types typically use the unit's plural name, or the non-plural name with the suffix `_t` (for type), e.g. `meters<int>` or `meter_t` (alias for `meters<double>`). See [the documentation](http://nholthaus.github.io/units/namespaces.html) for a complete list.

Units can (_and should!_) be used anywhere arithmetic literals can be used:

```cpp
double          area = 15 * 5 + 10 * 10;                // 175 m^2?
square_meter_t  area = 15_m * 5_m + 10_m * 10_m;        // 175 m^2
```

What makes unit types special is that unit conversions happen implicitly and automatically. Since unit conversions are evaluated at compile time, this means you can mix and match all the unit types you want with _no runtime penalty_.

```cpp
foot_t              len   = 5_m;                            // simple implicit conversion
meters_per_second_t speed = 60_mi / 1_hr;                   // more complex implicit conversion
square_meter_t      area  = 15_m * 5_m + 1000_cm * 1000_cm; // previous example with mixed units
```

Note the operators return the correct dimensions of area, even though the source types were all units of length. `<units.h>` has powerful dimensional analysis capabilities. But what happens if we get the return type wrong?

```cpp
meter_t  area = 15_m * 5_m + 10_m * 10_m;               // oops, m * m = m^2
```

> E:/workspace/units/include/units.h(1405): error C2338: Units are not compatible.

Your compiler will produce an "incompatible units" error if your dimensional analysis is incorrect. If your resulting unit types are complex, you could use `auto` for simplicity:

```cpp
auto result = 15_m * 5_m + 10_m * 10_m;                 //  m^2
auto speed  = 60_mi / 1_hr;                             //  60 mph
```

***NOTE: Think carefully about using `auto` for variables.*** When you explicitly specify the type, the compiler can check the dimensional analysis for correctness, and produce errors at compile time if you make a mistake. When using `auto`, you are basically saying that whatever unit initializer evaluates to is correct (even if it's not). If you are only using `auto` because a complex unit type is not available in the library, try [defining a new unit](#defining-new-units) as a better alternative.

More complex mathematical operations ([almost every `<cmath>` operation actually](http://nholthaus.github.io/units/namespaceunits_1_1math.html)), including exponentials and square roots are possibe by using the ADL-powered math functions.

```cpp
// Assume no `using namespace units;``

units::meter_t a = 3_m;
units::meter_t b = 4_m;
units::meter_t c = sqrt(pow<2>(a) + pow<2>(b)); // Pythagorean threorem.

std::cout << c << std::endl;                    // prints: "5 m"
```

# Unit initialization
<a id="markdown-unit-initialization" name="unit-initialization"></a>

There are several ways to initialize unit values:

- Explicit initialization

```cpp
meter_t       distance0(10);     // Explicit initialization of `double` unit from arithmetic literal
meters<float> distance1(10);     // Explicit initialization of `float`  unit from arithmetic literal
meter_t       distance2(10_m);   // Explicit initialization of `double` unit from unit literal
meter_t       distance3(100_ft); // Explicit initialization of `double` unit from convertible unit literal
```

- CTAD (Class Template Argument Deduction)

```cpp
meters distance0(10);       // `meters<int>`
meters distance0(10.0);     // `meters<double>`
meters distance0(10_m);     // `meters<int>`
meters distance0(100.0_ft); // `meters<double>`
```

# Conversion factors
<a id="markdown-conversion-factors" name="conversion-factors"></a>

Conversion factors are the foundation of the dimensional analysis of the units library. Conversion factors are tag types which are never instantiated in user code, but which provide the compile-time information about different units, including how to convert between them, and how to determine their compatibility for conversion.

All conversion factors are defined in namespaces nested within the `units` namespace, such as `units::length` or `units::angle`, to avoid name clashes between units of different physical quantities which share the same names (like pounds). SI base units are defined as "dimensions" in the `units::dimension` namespace.

Units are defined in terms of
 1. A scale factor relative to a base conversion factor.
 2. A base conversion factor
 3. [optionally] a scale factor of `pi`
 4. [optionally] a datum translation (such as the +/- 32 required to convert between `fahrenheit` and `celsius`)

Most conversion factors have their origin in the Scientific International (SI) base unit system. A special exception is made for angle units, which are defined in SI as ( m * m^-1), and in this library they are treated as a basic unit type because of their important engineering applications.

_Example_: the definitions of some common length conversion factors are:

```cpp
inline namespace length
{
	using meter_conversion_factor = conversion_factor<std::ratio<1>, units::dimension::length>; // meters are (1) unit of length in the SI system.
	using foot_conversion_factor  = conversion_factor<std::ratio<381, 1250>, meter_conversion_factor>; // feet are 0.3048 meters.
}
```

# Unit types
<a id="markdown-unit-types" name="unit-types"></a>

Unit types are the primary classes which will be used in user code. They can be thought of as essentially equivalent to a `double` (in the case of `_t` units), except that they have conversion factors associated with them. They can be used wherever a `double` would be used to store a dimensioned quantity. Unit types are derived from the `unit` class, and have the form `[singular-unit-name]_t`, e.g. `meter_t`, `radian_t` or `[plural-unit-name]<underlying-type>`, e.g. `meters<int>`, `radians<double>`.

Unit types are defined in terms of their conversion factor, their underlying type, and an optional non-linear scale (think decibels or Richter scale). For example, `meters` and `meter_t` could be defined as:

```cpp
template<class Underlying>
using meters = units::unit<units::meter_conversion_factor, Underlying, units::linear_scale>;

using meter_t = meters<double>;
```

or simply

```cpp
using meter_t = units::unit<units::meter_conversion_factor>;
```

since the underlying type and scale parameters default to `double` and `linear_scale` respectively.

Units of the same dimension (e.g length units) can be implicitly converted/assigned to one another if the conversion would be lossless (i.g. non narrowing nor truncating). Units (with the exception of dimensionless units) cannot be implicitly converted to/from built-in types, such as `double`.

Units are constructed from built-in types, and the `value()` method (or `operator()`) can be used to retrieve its underlying value. That said, the user should prefer to operate within the unit type-space as much as is practical, and wrappers of most `<cmath>` functions are provided to enable operating solely in the `unit` domain.

The primary purpose of unit types is to provide type safety and dimensional analysis for mathematical operations. For instance, the velocity of an object can be calculated:

```cpp
auto objectVelocity = meter_t(100.0) / second_t(2.0);
```

The resulting velocity type will be deduced to be `meters_per_second_t` with a value of 50.0. Additionally, if the return type if specified, the type system will verify that the units are compatible. For example, the following will fail to compile:

```cpp
units::meters_per_second_t objectVelocity = square_meter_t(100.0) / second_t(2.0); // Error: Unit types are not compatible.`
```

Unit types can (and should!) be used to perform implicit conversions:

```cpp
units::second_t a;
units::minute_t b(1.0);

a = b;	// a == 60.0
```

Arithmetic can be performed on unit types the same way it can for built-in types. However, unlike built-in types, the return value of unit-type arithmetic will be the proper unit to represent the resulting quantity.

```cpp
meter_t a_m(1.0), b_m(2.0), c_m;
foot_t	a_ft(1.0), b_ft(2.0), c_ft;

c_m = a_m + b_m;                            // OK. c == 3m
c_ft = a_m + b_m;                           // OK. resulting 3m is converted to ft.
auto result = a_m + b_ft;                   // OK. result is `meter_t` (left-most unit)

auto result_sm = a_m * b_m;                 // OK. result_sm is `square_meter_t`.
auto result_s = a_m / b_m;                  // OK. result_s is `dimensionless`.
auto result = a_m * b_ft;                   // OK. result is `square_meter_t` (left-most unit)

auto result = a_m * square_meter_t(1.0);    // OK. units can always be multiplied. Result is `cubic_meter_t`.
auto result = a_m * dimensionless(1.0);          // OK. units can always be multiplied. Result is `meter_t`.
```

Unsupported arithmetic, or improper return types will result in compiler errors:

```cpp
c_m = a_m + 5.0;                            // Error. can't add scalars to dimensioned units.
c_m = a_m + dimensionless(5.0);             // Error. can't add scalars to dimensioned units.
auto result = a_m + square_meter_t(1.0);    // Error. Incompatible units.
```

By providing explicit return types for unit functions, the compiler can be used to verify the accuracy of the dimensional analysis, and thus avoiding costly errors.

# Unit Literals
<a id="markdown-unit-literals" name="unit-literals"></a>

Unit literals can be a convenient way to initialize and work with unit values:

```cpp
using namespace units::literals;

meter_t dist	= 10_m;		// 10 m
meter_t dist2	= 1_km;		// 1000 m
```

Literals can also be used for any temporary values in calculations, making them more readable:

```cpp
auto area = units::meter_t(5) * units::meter_t(10); // without literals
auto area = 5_m * 10_m;                             // with literals
```

All literals* are defined by their SI abbreviation preceded by an underscore, e.g. `_m` for meter. "Square" units are preceded by `_sq`, e.g. `_sq_m` for square meters. Non SI units use their most common abbreviations.

All literals are defined in the `units::literals` namespace, and in order to use literals in your code ***you must include the statement `using namespace units::literals;`*** .

_* with the exception of `Teslas`, which use `_Te` for compatibility with MSVC compilers._

# `<cmath>` Functions
<a id="markdown-%3Ccmath%3E-functions" name="%3Ccmath%3E-functions"></a>

The `units` library includes unit type-safe wrapper versions for almost all of the `<cmath>` functions, _including_ the C++11 extensions. These functions can be found through ADL. The `units` library versions don't conflict with `<cmath>`, and it's possible to use both libraries in the same code.

The wrapped functions ensure that only the proper unit types are accepted into the functions, and that the return value type matches the expected units, all without needing to resort to the type-unsafe `value()` member.

In _rare_ cases, the overload resolution for a given type may be ambiguous. If so, simply fully-qualify the function with `units::`, e.g.

```cpp
meter_t x(2.0);
meter_t y(3.0);
square_meter_t z(1.0);

double         result = fma(x, y, z);                         // Error: ambiguous
double         result = fma(x.value(), y.value(), z.value()); // Unsafe!
square_meter_t result = math::fma(x, y, z);                   // OK.
```

# Exponentials and Square Roots
<a id="markdown-exponentials-and-square-roots" name="exponentials-and-square-roots"></a>

Many functions require units to be raised to some power. This can be accomplished using the `units::pow` function:

```cpp
square_meter_t m2 = units::pow<2>(meter_t(5.0));	// m2 == 25.0
```

The only constraint is that the exponential power (given in the template argument) must be known at compile time, so that the type system can deduce the output type. This differs from the `<cmath> pow` implementation, which takes exponent values at runtime.

Square roots are also provided with the `units::sqrt` function. Due to the nature of the `sqrt` operation, the units library can often provide exact conversions for square root operations, but _not in every case_. The rest of the time, the `sqrt` unit will be a _rational approximation_ of the real value. These are guaranteed to be accurate to at least 10 decimal places.

```cpp
meter_t m = units::sqrt(square_meter_t(4.0));		// m == 2.0
```

# Removing type safety
<a id="markdown-removing-type-safety" name="removing-type-safety"></a>

When interfacing with APIs, libraries, and frameworks which aren't `unit` enabled, it may be necessary (if regrettable) to remove the type-safety of a unit type and expose its underlying value. This is possible using the `unit_cast` function, or the `to<>` member function.

```cpp
using namespace units;

// Get double value from a unit type (`double` is the default underlying type of `_t` units)
meter_t dist(10);
double dval = unit_cast<double>(dist);
double dval2 = dist.to<double>();

// Get integer value (potentially narrowing, be careful!)
int ival = unit_cast<int>(dist);
int ival2 = dist.to<int>();
```

Both functions produce the same results, the choice of syntax is simply a user preference.

To determine the underlying type of the unit container, the (verbose) trait `units::traits::unit_traits<decltype(dist)>::underlying_type` could be used.

# Efficiency
<a id="markdown-efficiency" name="efficiency"></a>

Complex, recurively-defined conversions are performed in just 5 instructions:

		year_t twoYears(2.0);
		week_t twoYearsInWeeks = twoYears;
	00007FF7BDB57FF6  xorps       xmm9,xmm9
	00007FF7BDB57FFA  cvtsi2sd    xmm9,rax
	00007FF7BDB57FFF  mulsd       xmm9,mmword ptr [__real@4000000000000000 (07FF7BDBB31A0h)]
	00007FF7BDB58008  divsd       xmm9,mmword ptr [__real@401c000000000000 (07FF7BDBB33C0h)]
	00007FF7BDB58011  movsd       mmword ptr [rbp+6Fh],xmm9
		EXPECT_EQ(week_t(104.286), twoYearsInWeeks);
	00007FF7BDB58017  ...

In the library, the year to week conversion is defined in terms of
`years -> days -> hours -> minutes -> seconds -> minutes -> hours -> days -> weeks`
but the total conversion ratio is computed at compile-time and the math is optimized to two floating-point operations.

Unit conversions between equivalent types are optimized away completely, and generate _no machine code_.

# `constexpr` support
<a id="markdown-constexpr-support" name="constexpr-support"></a>

In many cases, unit equations are used to determine derived values from a set of values which are known at compile-time. In these situations, it would be optimal to pre-compute the derived values _at compile time_, thus generating no machine code and incurring no run-time penalty.

The unit types of `units` are mostly `constexpr` enabled to perform compile-time arithmetic. The limits are the `<cmath>` wrappers, which wrap non-`constexpr` `<cmath>` functions, and the decibel units which use them.

# Conversion without unit types
<a id="markdown-conversion-without-unit-types" name="conversion-without-unit-types"></a>

The preferred method of conversion is implicitly though the use of unit types, however unit conversion can be accomplished using `units::convert` for arithmetic types:

```cpp
double val_in = convert<feet_conversion_factor, inch_conversion_factor>(1.0);	// val_in == 12.0
```

For type-safe conversion, prefer implicit conversion via unit type construction/assignment.

# Namespaces
<a id="markdown-namespaces" name="namespaces"></a>

The following namespaces are nested within the `units` namespace.

Unit types and conversion factors are split into per-dimension inline namespaces to avoid conflicting unit names which represent different physical quantities.

`units::constants`: inline namespace with definitions of scalar and non-scalar physical constants like Avogadro's number.

`units::literals`: namespace with definitions of the literal operators for unit types.

The ADL-enabled mathematical operations, like `sin`, `log`, `floor`, etc are defined in `units`.

`units::traits`: namespace with definitions of type traits that you can use to query unit types.

# Defining new units
<a id="markdown-defining-new-units" name="defining-new-units"></a>

The units library strives to provide unit types for every conceivable unit, and before defining your own units you should double-check the headers to make sure it's not already included. That said, if you need to roll your own units, the library is extensible by design.

Defining new conversion factors is simple, as they can be recursively defined as ratio of previously-defined conversion factors in a way that mimics natural language and is highly readable:

```cpp
inline namespace time
{
	using second_conversion_factor = units::conversion_factor<std::ratio<1>,   units::dimension::time>;
	using minute_conversion_factor = units::conversion_factor<std::ratio<60>,  second_conversion_factor>;
	using hour_conversion_factor   = units::conversion_factor<std::ratio<60>,  minute_conversion_factor>;
	using day_conversion_factor    = units::conversion_factor<std::ratio<24>,  hour_conversion_factor>;
	using week_conversion_factor   = units::conversion_factor<std::ratio<7>,   day_conversion_factor>;
	using year_conversion_factor   = units::conversion_factor<std::ratio<365>, week_conversion_factor>;
}
```

Conversion factors are defined in the form: `using [conversion-factor] = units::conversion_factor<std::ratio<[ratio of base units per unit]>, [base unit]>;`, where:
 - the `[conversion-factor]` is what you are defining.
 - the `[base unit]` is the conversion factor that `[conversion-factor]` will be defined in terms of, and
 - the `[number of base units per unit]` is the conversion ratio between the two, expressed as a `std::ratio` type.

Compound conversion factors are defined in a similar manner, with additional helper functions for polynomials:

    ```cpp
	using acceleration = compound_conversion_factor<meter_conversion_factor, inverse<squared<second_conversion_factor>>>;		// (m / s^2)
    ```

The available helpers are:
 - `units::inverse<...>`     (inverts the conversion factor, e.g. meters becomes meters^-1, or 1 / meters)
 - `units::squared<...>`     (squares the conversion factor, e.g. meters becomes meters^2)
 - `units::cubed<...>`       (cubes the conversion factor, e.g. meters becomes meters^3)
 - `units::square_root<...>` (takes the square root of the conversion factor, e.g meters^2 becomes meters)
 - `units::atto<...>` through `units::exa<...>` metric prefixes

# Unit definition macros
<a id="markdown-unit-definition-macros" name="unit-definition-macros"></a>

Version `2.1.0` of the units library simplifies the task of adding new units by introducing a set of macros for unit definitions:

- `UNIT_ADD(namespaceName, nameSingular, namePlural, abbreviation, definition)`

  This macro adds a single new unit to the given namespace, as well as a literal definition and `cout` support based on the given `abbreviation`. e.g.

  ```cpp
  UNIT_ADD(length, foot, feet, ft, unit<std::ratio<381, 1250>, meters>)
  ```

  Would create the `units::length::feet` tag, the `units::length::foot_t` container type, and the `_ft` literal.

- `UNIT_ADD_WITH_METRIC_PREFIXES(namespaceName, nameSingular, namePlural, abbreviation, definition)`

  This macro has the same functionality as `UNIT_ADD`, but additionally adds unit types with all metric prefixes from `femto` to `peta` (smaller and larger prefixes mostly result in arithmetic overflow).

- `UNIT_ADD_WITH_CUSTOM_TYPE(namespaceName, nameSingular, namePlural, abbreviation, underlyingType, definition)`

  This macro has the same functionality as `UNIT_ADD`, but additionally adds an `underlyingType` parameter, which can be used to create units with integral, or other underlying types. The library default underlying type is `double`.
- `UNIT_ADD_DECIBEL(namespaceName, nameSingular, abbreviation)`

  Adds the decibel representation for a previously-defined unit. e.g.

  ```cpp
  UNIT_ADD_DECIBEL(power, watt, dBW)
  ```

  Adds the `dBW_t` container, and the `_dBW` literal.

- `UNIT_ADD_CATEGORY_TRAIT(unitCategory, baseUnit)`

  This macro creates a type-trait to check whether a unit is of a certain category, e.g. length. This is only necessary if defining new categories of units which are not included in `units.h` at all. e.g.

  ```cpp
  UNIT_ADD_CATEGORY_TRAIT(length, meter)
  ```

  Adds the `units::traits::is_length_unit` trait.

# Unit Type Traits
<a id="markdown-unit-type-traits" name="unit-type-traits"></a>

The units library provides a comprehensive set of type-traits, which can be used in templated user code to enforce that the unit types have certain properties.

For example, let's say you want to write a function that validates that the square footage of an office (given in any units), meets the minimum size required by local ordinance.

```cpp
template<typename Units>
bool isMinimumSize(Units x)
{
	return x >= square_feet_t(80.0);
}
```

This function will fail to compile if `Units` is not a unit of area (since incompatible unit types are not comparable), but it will produce a series difficult-to-understand template errors. Type traits could be used to make the error message more friendly:

```cpp
template<typename Units>
bool isMinimumSize(Units x)
{
	static_assert(units::traits::is_area_unit_v<Units>, "Input value x must represent an area quantity.");
	return x >= square_feet_t(80.0);
}
```

See the `units::traits` namespace for a list of all the supported traits.

# Underlying type of units
<a id="markdown-underlying-type-of-units" name="underlying-type-of-units"></a>

The default underlying type for `_t` units is `double`.
However, you can use the plural-named units, which are template entities,
to specify the underlying type as the template argument.
For example, `1_m` has type `meters<int>`,
while `1.0_m` has type `meter_t`, which is an alias for `meters<double>`.

# Disabling IOStream
<a id="markdown-disabling-iostream" name="disabling-iostream"></a>

For some embedded applications, it may be [desirable to remove all references to `<iostream>` in order to reduce compiled binary size and RAM requirements](https://github.com/nholthaus/units/issues/32). There are two ways to accomplish this:

1. If you are copy/pasting the `include` directory contents into your project's include directory, then simply define `UNIT_LIB_DISABLE_IOSTREAM` before including this library's headers.

   ```cpp
   #define UNIT_LIB_DISABLE_IOSTREAM
   #include <units.h>
   ```

2. If you are including `units` in your project as a `CMake` target (using `add_subdirectory`), then all you need to do is set the `DISABLE_IOSTREAM` cache option, either using the cmake-gui, or by adding the option to the cmake command line during configuration:

   ```bash
   cmake -DDISABLE_IOSTREAM=ON -DBUILD_TESTS=OFF ..
   cmake --build . --config Release
   ```

# Improve compilation time
<a id="markdown-improve-compilation-time" name="improve-compilation-time"></a>

# MSVC compile options
<a id="markdown-msvc-compile-options" name="msvc-compile-options"></a>

enabling parallel builds in CMake:

`set(ENV{CL} /MP)`

and disabling microsoft-specific compiler extensions:

`target_compile_definitions(${PROJECT_NAME} INTERFACE "/permissive-")`

can improve the build times by a substantial margin (up to 5x in the authors testing).

## Per-Dimension Headers
<a id="markdown-per-dimension-headers" name="per-dimension-headers"></a>

If you know that you only need a subset of the units for your application,
you can use the per-dimension headers for units.
For example, `#include <units/length.h>` will only give you access to the units of length dimension.

# Macro clashes
<a id="markdown-macro-clashes" name="macro-clashes"></a>

With certain compilers, it is possible that system header files like `<ctype.h>` will define macros which conflict with the unit literals, which use SI abbreviations. In these cases, it is general safe and advisable to `#undef` the offending macros.

## Windows macros
<a id="markdown-windows-macros" name="windows-macros"></a>

`_T` is known to conflict, but is hardcoded into the compiler and can't be disabled. For this reason, `Tesla` units use the `_Te` abbreviation.

The following macros may need to be undefined on the Windows platform to use `units`:

   ```cpp
   #undef pascal
   #include <units.h>
   ```

## ARM macros
<a id="markdown-arm-macros" name="arm-macros"></a>

The following macros may need to be undefined on the ARM platform to use `units::literals`:

   ```cpp
   #undef _U
   #undef _L
   #undef _N
   #undef _S
   #undef _P
   #undef _C
   #undef _X
   #undef _B
   #define UNIT_LIB_DISABLE_IOSTREAM // it's prudent to disable IOStream on embedded platforms as well.
   #include <units.h>
   ```

   It's best to undefine macros on an as-needed basis.

# CMake Instructions
<a id="markdown-cmake-instructions" name="cmake-instructions"></a>

There are several ways to incorporate `<units.h>` into your project. The simplest is to just copy `include/*` into your project include directory (which the licensing allows you to do). However, you'll have to properly set up the necessary compilation flags for C++17 (`-std=c++17` on gcc).

However, if you are already using CMake as your build system, the recommended way to include `units` is to copy the entire `units` project as a subdirectory within your own top-level project folder. Then, in your CMakeLists.txt file add

   ```cmake
   add_subdirectory(units)

   add_executable(${PROJECT_NAME} main.cpp)
   target_link_libraries(${PROJECT_NAME} units)
   ```

Also, if you are distributing headers that depends on `<units.h>`, you shoud consider using cmake's `find_package` to check if the header is installed on the user's system:

    ```cmake
    find_package(units)

    add_library(${PROJECT_NAME} my_lib.cpp)
    target_link_libraries(${PROJECT_NAME} units::units)
    ```

The include path properties are part of the `units` target, so adding it as a subdirectory and linking against it is all you need to do, no need to worry about additional include directories.

If you don't care about the unit tests, you can minimize compile time by invoking CMake with the following option:

   ```bash
   cmake -DBUILD_TESTS=OFF ..
   cmake -build .
   ```

# Build Instructions
<a id="markdown-build-instructions" name="build-instructions"></a>

The library itself consists of an all-including header [`<units.h>`](include/units.h) and finer-grained headers in [include/units/](include/units/), and can be included into your project without being built.

The unit tests and documentation can be built with CMake. A doxygen installation is required to generate the documentation, and a Tex install is needed if pdf documentation is desired.

To build the tests:

## Windows
<a id="markdown-windows" name="windows"></a>

 1. Ensure [`cmake`](https://cmake.org/download/) is installed, and that the `bin` directory is in your `%PATH%` variable, and that a compiler like [`Visual Studio 2015 Community Edition`](https://www.visualstudio.com/post-download-vs?sku=community&clcid=0x409&downloadrename=true#) is installed.
 2. clone the repository or download the `.zip` package.
 3. Open a `cmd` terminal and navigate to the source directory.
 4. Type the following commands:
   - `md build`
   - `cd build`
   - `cmake -Wno-dev ..`
   - `cmake --build . --config Release`
 5. The tests will be created in an executable called `unitLibTest.exe` in the folder `build/unitTests/Release`.

## Linux
<a id="markdown-linux" name="linux"></a>

 1. Ensure you are using cmake 3.2 or later. You can verify this with `cmake --version`.
 2. Ensure you are using gcc version 7 or greater. You can verify this with `gcc --version`.
 3. clone the repository or download the `.tar.gz` package.
 4. Open a terminal and navigate to the source directory.
 5. Type the following commands:
   - `mkdir build`
   - `cd build`
   - `cmake -Wno-dev ..`
   - `cmake --build . --config Release`
 6. The tests will be created in an executable called `unitLibTest` in the folder `build/unitTests`.

# Previous Releases
<a id="markdown-previous-releases" name="previous-releases"></a>

 - [`v2.0.3`](https://github.com/nholthaus/units/releases/tag/v2.0.3)
   - `unit_t` types are now trivial types.
   - `unit_t` types support the unary minus (negation) operator.
   - Compile-time unit arithmetic via `unit_value_t`.
   - Unit-enabled ports of most `<cmath>` functions, including c++11 extensions.
   - Square-root manipulators for `unit`, `unit_t`, and `unit_value_t`.
   - Improved documentation.
 - [`v1.3.0`](https://github.com/nholthaus/units/releases/tag/v1.3.0)
   - Adds ostream support.
   - bug fixes.
 - [`v1.2.2`](https://github.com/nholthaus/units/releases/tag/v1.2.2)
   - Bug fixes (#1) and namespace cleanup.
 - [`v1.2.0`](https://github.com/nholthaus/units/releases/tag/v1.2.0)
   - Adds angular velocity units.
 - [`v1.1.1`](https://github.com/nholthaus/units/releases/tag/v1.1.1)
   - Adds Doxygen and additional type traits.
 - [`v1.0.0`](https://github.com/nholthaus/units/releases/tag/v1.0.0)
   - Initial release.
