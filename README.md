
# UNITS 
A compile-time, header-only, dimensional analysis library built on c++14 with no dependencies.

[![Linux build](https://travis-ci.org/nholthaus/units.svg?branch=master)](https://travis-ci.org/nholthaus/units) [![Windows build](https://ci.appveyor.com/api/projects/status/github/nholthaus/units?svg=true&branch=master)](https://ci.appveyor.com/project/nholthaus/units) [![Coverage Status](https://coveralls.io/repos/github/nholthaus/units/badge.svg?branch=master)](https://coveralls.io/github/nholthaus/units?branch=master) ![license](https://img.shields.io/badge/license-MIT-orange.svg) ![copyright](https://img.shields.io/badge/%C2%A9-Nic_Holthaus-orange.svg) ![language](https://img.shields.io/badge/language-c++-blue.svg) ![c++](https://img.shields.io/badge/std-c++14-blue.svg)<br>![msvc2015](https://img.shields.io/badge/MSVC-2015-ff69b4.svg) ![msvc2017](https://img.shields.io/badge/MSVC-2017-ff69b4.svg) ![gcc-4.9.3](https://img.shields.io/badge/GCC-4.9.3-ff69b4.svg) ![gcc-5.4.0](https://img.shields.io/badge/GCC-5.4.0-ff69b4.svg) ![clang-3.4](https://img.shields.io/badge/CLANG-3.4-ff69b4.svg)

# Get in touch

If you are using `units.h` in production code, I'd love to hear from you via GitHub issues!

# Latest Release - v2.3.1

## Get it
[![DOWNLOAD](https://img.shields.io/badge/Download-v2.3.1-green.svg)](https://github.com/nholthaus/units/releases/tag/v2.3.1)

## New in v2.3.1

**This version removes support for the Visual Studio 2013 compiler.**

Features:
- units now include constexpr `name()` and `abbreviation()` member functions, which do not really on string/iostream.
- Builds with VS2017 Ninja generator out of the box
- string conversions are now locale aware
- added unary increment and decrement operators (`++`,`--`), as well as unary `+` operator.

Bug fixs:
- fixed compilation error when iostream was disabled

## New in v2.3.0

Features:
- 5x compile time improvement on MSVC.
- 1.5x compile time improvement on GCC.
- Even more dramatic reductions in compile time can be achieved if you opt-in to specific unit definitions instead of using all the library-defined types (which is the default value). Check out [Enabling a subset of units to improve compilation time](#enabling-a-subset-of-units-to-improve-compilation-time) for instructions.
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
- Fixed singular name of `siemen` to be `siemens` (Thanks @Oxyd)
- Fixed bug with `cubrt` operation (Thanks @PearCoding)
- Fixed constexpr relational operators bug
- Fixed exponential temperature conversions (Thanks @guarndt)
 
## Tested on

 - gcc-4.9.3
 - gcc-5.4.0
 - clang-3.4
 - msvc2015
 - msvc2017

Does this library work on your compiler? If so, let me know!

# Contents

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
- [Unit tags](#unit-tags)
- [Unit containers](#unit-containers)
- [Unit Literals](#unit-literals)
- [`<cmath>` Functions](#cmath-functions)
- [Exponentials and Square Roots](#exponentials-and-square-roots)
- [Removing type safety](#removing-type-safety)
- [Efficiency](#efficiency)
- [Pure Compile-time Unit Manipulation](#pure-compile-time-unit-manipulation)
- [Conversion without unit containers](#conversion-without-unit-containers)
- [Namespaces](#namespaces)
- [Defining new units](#defining-new-units)
- [Unit definition macros](#unit-definition-macros)
- [Unit Type Traits](#unit-type-traits)
- [Changing the underlying type of `unit_t`](#changing-the-underlying-type-of-unit_t)
- [Disabling IOStream](#disabling-iostream)
- [Enabling a subset of units to improve compilation time](#enabling-a-subset-of-units-to-improve-compilation-time)
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

[The full documentation is available ***here***](http://nholthaus.github.io/units).

# Description

The library consists of a single file ([units.h](include/units.h)), plus unit tests. To incorporate the library into your project, simply copy the header into a location in your include path, or add the [included CMake project](#cmake-instructions) into your build. Using the CMake project, you can also build the unit tests and documentation if desired.

The library provides a set of types, containers, and traits to solve dimensional analysis problems, that is, problems involving dimensioned physical quantities. The conversions between units are defined as ratios at compile time, making the library _incredibly_ fast. Additionally, specifying units as _types_, rather than variable suffixes (or not at all), provides complete type-safety within the compiler. This means that code that accidentally misuses units or which has errors in the dimensional analysis _will fail at compile-time, not at run-time_. 

The unit test file `unitTests/main.cpp` contains example usage of every type, trait, and function contained in the library, and while not exactly user-friendly, can be a valuable resource.

# Getting started guide

Add `units.h` to your project, along with the `using` directive for literals

```cpp
#include <units.h>

using namespace units::literals;
```

Each "dimension" of unit is defined in its own namespace. See [the namespaces section](#namespaces) for a complete list. The rest of the guide assumes you've included the namespaces you plan to use:

```cpp
using namespace units;
using namespace units::length;
using namespace units::time;
using namespace units::area;
using namespace units::velocity;
```

**The easiest way to get started with the `units` library is to think of unit containers as `double` values.** Unit containers are typically the units' non-plural name with the suffix `_t` (for type), e.g. `meter_t`. See [the documentation](http://nholthaus.github.io/units/namespaces.html) for a complete list.

Units can (_and should!_) be used anywhere `double` values can be used:

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

Note the return type has the correct dimensions of area, even though the source types were all units of length. `units.h` has powerful dimensional analysis capabilities. But what happens if we get the return type wrong?

```cpp
meter_t  area = 15_m * 5_m + 10_m * 10_m;               // oops, m * m = m^2
```

> E:/workspace/units/include/units.h(1405): error C2338: Units are not compatible.

Your compiler will produce an "incompatible units" error if your dimensional analysis is incorrect. If your resulting unit types are complex, you could use `auto` for simplicity:

```cpp
auto result = 15_m * 5_m + 10_m * 10_m;                 //  m^2
auto speed  = 60_mi / 1_hr;                             //  60 mph
```

***NOTE: Think carefully about using `auto` for return types.*** When you explicitly declare the return type, the compiler can check the dimensional analysis for correctness, and produce errors at compile time if you make a mistake. When using `auto`, you are basically saying that whatever unit the right-hand side of the expression results to is correct (even if it's not). If you are only using `auto` because a complex unit type is not available in the library, try [defining a new unit](#defining-new-units) as a better alternative.

More complex mathematical operations ([almost every `<cmath>` operation actually](http://nholthaus.github.io/units/namespaceunits_1_1math.html)), including exponentials and square roots are possibe by using the `units::math` namespace .

```cpp
using namespace units::math;

meter_t a = 3_m;
meter_t b = 4_m;
meter_t c = sqrt(pow<2>(a) + pow<2>(b));    // Pythagorean threorem.

std::cout << c << std::endl;                // prints: "5 m"
```
# Unit initialization

There are several ways to initialize unit values:

- Explicit initialization

  ```cpp
  meter_t distance_m(10); // Explicit initialization from double
  meter_t distance(10_m); // Explicit initialization from unit literal
  meter_t dist(100_ft);   // Explicit initialization from unit literal of a different type
  ```

- `make_unit<...>()` factory. The syntax is familiar to `boost::units` users, and allows explicit reference to the unit type for member variable initialization.

   ```cpp
   class myClass
   {
     public:
       
       myClass() : m_speed(make_unit<miles_per_hour_t>(100)) {}

     private:

       miles_per_hour_t m_speed;
   };
   ```

# Unit tags
Unit tags are the foundation of the unit library. Unit tags are types which are never instantiated in user code, but which provide the meta-information about different units, including how to convert between them, and how to determine their compatibility for conversion.

All unit tags are defined in namespaces under the `units` namespace, such as `units::length` or `units::angle`, to avoid name clashes between units of different physical quantities which share the same names (like pounds). SI base units are defined as "categories" in the `unit` namespace.

Units are defined in terms of 
 1. A scale factor relative to a base unit type.
 2. A base unit
 3. [optionally] a scale factor of `pi`
 4. [optionally] a datum translation (such as the +/- 32 required to convert between `fahrenheit` and `celsius`)

 All units have their origin in the Système International (SI) base unit system. A special exception is made for angle units, which are defined in SI as ( m * m^-1), and in this library they are treated as a basic unit type because of their important engineering applications.
 
_Example_: the definitions of some common length units are:

```cpp
namespace length
{
	using meters = units::unit<std::ratio<1>, units::category::length_unit>;	// meters are (1) unit of length in the SI system.
	using feet = units::unit<std::ratio<381, 1250>, meters>;					// feet are 0.3048 meters.
}
```

# Unit containers

Unit containers are the primary classes which will be instantiated in user code.  They can be thought of as essentially equivalent to a `double`, except that they have unit type tags associated with them. They can be used wherever a double would be used to store a dimensioned quantity. Containers are derived from the `unit_t` class, and have the form `[unitname]_t`, e.g. `meter_t` or `radian_t`.

Unit containers are defined in terms of the units they represent, their underlying type, and an optional non-linear scale (think decibels or Richter scale). For example, `meter_t` would be defined: 

```cpp
using meter_t = units::unit_t<units::length::meter, double, units::linear_scale>
```

or simply 

```cpp
using meter_t = units::unit_t<units::length::meter>
```

since the underlying type and scale parameters default to `double` and `linear_scale` respectively.

Units of compatible types (e.g length units) can be implicitly converted/assigned to one another. Units (with the exception of dimensionless types) cannot be implicitly converted to/from built-in types, such as `double`. 

Units are constructed from built-in types, and the `toDouble()` method (or `operator()`) can be used to retrieve a built-in type value. That said, the user should prefer to operate within the unit type-space as much as is practical, and wrappers of most `<cmath>` functions are provided to enable operating solely in the `unit_t` domain. 

The primary purpose of unit containers is to provide type safety and dimensional analysis for mathematical operations. for instance, the velocity of an object can be calculated:

```cpp
auto objectVelocity = meter_t(100.0) / second_t(2.0);
```

The resulting velocity type will be deduced to be `velocity::meters_per_second` with a value of 50.0. Additionally, if the return type if specified, the type system will verify that the units are compatible. For example, the following will fail to compile:

```cpp
units::velocity::meters_per_second objectVelocity = square_meter_t(100.0) / second_t(2.0); // Error: Unit types are not compatible.`
```

Unit containers can (and should!) be used to perform implicit conversions:

```cpp
units::time::second_t a;
units::time::minute_t b(1.0);

a = b;	// a == 60.0
```

Arithmetic can be performed on unit containers the same way it can for built-in types. However, unlike built-in types, the return value of unit-type arithmetic will be the proper unit to represent the resulting quantity.

```cpp
using namespace units::length;
using namespace units::area;

meter_t a_m(1.0), b_m(2.0), c_m;
foot_t	a_ft(1.0), b_ft(2.0), c_ft;

c_m = a_m + b_m;                            // OK. c == 3m
c_ft = a_m + b_m;                           // OK. resulting 3m is converted to ft.
auto result = a_m + b_ft;                   // OK. result is `meter_t` (left-most unit)

auto result_sm = a_m * b_m;                 // OK. result_sm is `square_meter_t`.
auto result_s = a_m / b_m;                  // OK. result_s is `dimensionless_t`.
auto result = a_m * b_ft;                   // OK. result is `square_meter_t` (left-most unit)

auto result = a_m * square_meter_t(1.0);    // OK. units can always be multiplied. Result is `cubed<meter_t>`.
auto result = a_m * scalar_t(1.0);          // OK. units can always be multiplied. Result is `meter_t`.
```
	
Unsupported arithmetic, or improper return types will result in compiler errors:

```cpp
c_m = a_m + 5.0;                            // Error. can't add scalars to dimensioned units.
c_m = a_m + scalar_t(5.0);                  // Error. can't add scalars to dimensioned units.
auto result = a_m + square_meter_t(1.0);    // Error. Incompatible units.
```

By providing explicit return types for unit functions, the compiler can be used to verify the accuracy of the dimensional analysis, and thus avoiding costly errors.

# Unit Literals

If you are using a compiler which supports user-defined literals (e.g. not Visual Studio 2013), then unit literals can be a convenient way to initialize and work with unit values:

```cpp
using namespace units::literals;

meter_t dist	= 10_m;		// 10 m
meter_t dist2	= 1_km;		// 1000 m
```

Literals can also be used for any temporary values in calculations, making them more readable:

```cpp
auto area = units::length::meter_t(5) * units::length::meter_t(10);	// without literals
auto area = 5_m * 10_m;							// with literals
```

All literals* are defined by their SI abbreviation preceded by an underscore, e.g. `_m` for meter. "Square" units are preceded by `_sq`, e.g. `_sq_m` for square meters. Non SI units use their most common abbreviations.

All literals are defined in the `units::literals` namespace, and in order to use literals in your code ***you must include the line `using units::literals`*** (since there is no way to put a namespace on an operator).

_* with the exception of `Teslas`, which use `_Te` for compatibility with MSVC compilers._

# `<cmath>` Functions

The `units` library include type-safe unit_t container wrappers for almost all of the `<cmath>` functions, _including_ the c++11 extensions. These functions can be found in the `units::math` namespace. The `units` library versions don't conflict with `<cmath>`, and it's possible to use both libraries in the same code. 

The overloaded functions ensure that only the proper unit types are accepted into the functions, and that the return value type matches the expected units, all without needing to result to the type-unsafe `toDouble()` member.

In _rare_ cases, the overload resolution for a given type may be ambiguous. If so, simply prepend the function with the fully-qualified `units::math` prefix, e.g.

```cpp
meter_t x(2.0);
meter_t y(3.0);
square_meter_t z(1.0);
square_meter_t result;

result = fma(x, y, z);												// Error: ambiguous
double result = fma(x.toDouble(), y.toDouble(), z.toDouble());		// Warning: Unsafe!
result = math::fma(x, y, z);										// OK.
```

# Exponentials and Square Roots

Many functions require units to be raised to some power. This can be accomplished using the `units::math::pow` function:

```cpp
square_meter_t m2 = units::math::pow<2>(meter_t(5.0));	// m2 == 25.0
```

The only constraint is that the exponential power (given in the template argument) must be known at compile time, so that the type system can deduce the output type. This differs from the `<cmath> pow` implementation, which takes exponent values at runtime.

Square roots are also provided with the `units::math::sqrt` function. Due to the nature of the `sqrt` operation, the units library can often provide exact conversions for square root operations, but _not in every case_. The rest of the time, the `sqrt` unit will be a _rational_approximation_ of the real value. These are guaranteed to be accurate to at least 10 decimal places.

```cpp
meter_t m = units::math::sqrt(square_meter_t(4.0));		// m == 2.0
```

# Removing type safety

When interfacing with APIs, libraries, and frameworks which aren't `unit` enabled, it may be necessary (if regrettable) to remove the type-safety of a unit container and expose its underlying type. This is possible using the `unit_cast` function, or the `to<>` member function.

```cpp
using namespace units;
using namespace units::length;

// Get double value from a unit container (double is the default underlying type of the units library)
meter_t dist(10);
double dval = unit_cast<double>(dist);
double dval2 = dist.to<double>();

// Get integer value (potentially narrowing, be careful!)
int ival = unit_cast<int>(dist);
int ival2 = dist.to<int>();
```

Both functions produce the same results, the choice of syntax is simply a user preference.

To determine the underlying type of the unit container, the (verbose) trait `units::traits::unit_t_traits<decltype(dist)>::underlying_type` could be used.

# Efficiency

Complex, recursively-defined conversions are performed in just 5 instructions:

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

# Pure Compile-time Unit Manipulation

In many cases, unit equations are used to determine derived values from a set of values which are known at compile-time. In these situations, it would be optimal to pre-compute the derived values _at compile time_, thus generating no machine code and incurring no run-time penalty.

The `unit_value_t` class is the mechanism in the units library to perform compile-time arithmetic. The `unit_value_t` class functions exactly the same way as `std::ratio`, but with an associated unit tag and the ensuing type safety.

For a simple example, let's define a right triangle whose hypotenuse is the sum of the squares of its side (a Pythagorean triple)

```cpp
struct RightTriangle
{
	using a = unit_value_t<meters, 3>;
	using b = unit_value_t<meters, 4>;
	using c = unit_value_sqrt<unit_value_add<unit_value_power<a, 2>, unit_value_power<b, 2>>>;
};
```
	
The definition above is perfectly efficient, as it generates _no run-time code_ whatsoever, and still provides all the type safety of unit containers. The values of `a`, `b`, and `c` can be accessed at runtime using the static `value()` method of `unit_value_t`

```cpp
auto a = RightTriangle::a::value();	// a is `meter_t(3)`
auto b = RightTriangle::b::value();	// b is `meter_t(4)`
auto c = RightTriangle::c::value();	// c is `meter_t(5)`
```

The available compile-time operations are:

 - `units::unit_value_add`
 - `units::unit_value_subtract`
 - `units::unit_value_multiply`
 - `units::unit_value_divide`
 - `units::unit_value_power`
 - `units::unit_value_sqrt`
 
# Conversion without unit containers

The preferred method of conversion is implicitly though the use of unit containers, however unit conversion can be accomplished using `units::convert` for arithmetic types:

```cpp
double val_in = convert<feet, inches>(1.0);	// val_in == 12.0
```
	
For type-safe conversion, prefer implicit conversion via unit_t type containers..

# Namespaces

Unit tags and containers are split into separate namespaces to avoid conflicting unit names which represent different physical quantities.

Unit tag and `unit_t` container definitions are defined in the following namespaces:
 - units::length
 - units::mass
 - units::time
 - units::angle (plane)
 - units::current
 - units::temperature
 - units::substance (amount of, i.e. moles)
 - units::luminous_intensity
 - units::solid_angle
 - units::frequency
 - units::velocity
 - units::angular_velocity
 - units::acceleration
 - units::force
 - units::pressure
 - units::charge
 - units::energy
 - units::power
 - units::voltage
 - units::capacitance
 - units::impedance
 - units::magnetic_flux
 - units::magnetic_field_strength
 - units::inductance
 - units::luminous_flux
 - units::illuminance
 - units::radiation
 - units::torque
 - units::area
 - units::volume
 - units::density
 - units::concentration
 - units::data
 - units::data_transfer_rate
 - units::constants (scalar and non-scalar physical constants like Avogadro's number)
 
Literal values for unit containers are defined in the `literals` namespace
- units::literals

Mathematical operations like `sin`, `log`, `floor`, etc are defined in the following namespaces:
 - units::math
 
Type traits that you can use to test unit types are defined in the following namespaces:
 - units::traits

# Defining new units

The units library strives to provide built-in types for every conceivable unit, and before defining your own units you should double-check the namespaces to make sure it's not already included. That said, if you need to roll your own units, the library is extensible by design.

Defining new units is simple, as they can be recursively defined as ratio of previously-defined units in a way that mimics natural language and is highly readable:

```cpp
namespace time
{
	using seconds = units::unit<std::ratio<1>,   units::category::time_unit>;
	using minutes = units::unit<std::ratio<60>,  seconds>;
	using hours   = units::unit<std::ratio<60>,  minutes>;
	using days    = units::unit<std::ratio<24>,  hours>;
	using weeks   = units::unit<std::ratio<7>,   days>;
	using years   = units::unit<std::ratio<365>, days>;
}
```

Units are defined in the form: `using [unit] = unit<std::ratio<[number of base units per unit]>, [base unit]>;`, where:
 - the `[unit]` is what you are defining.
 - the `[base unit]` is the unit that `[unit]` will be defined in terms of, and
 - the `[number of base units per unit]` is the conversion ratio between the two, expressed as a `std::ratio` type.
 
Compound units are defined in a similar manner, with additional helper functions for polynomials:

	using acceleration = compound_unit<meters, inverse<squared<seconds>>>;		// (m / s^2)
	
The available helpers are:
 - `units::inverse<...>`     (inverts the unit, e.g. meters becomes meters^-1, or 1 / meters)
 - `units::squared<...>`     (squares the unit, e.g. meters becomes meters^2)
 - `units::cubed<...>`       (cubes the unit, e.g. meters becomes meters^3)
 - `units::square_root<...>` (takes the square root of the unit, e.g meters^2 becomes meters)
 - `units::atto<...>` through `units::exa<...>` metric prefixes
	
# Unit definition macros

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
	static_assert(units::traits::is_area_unit<Units>::value, "Input value x must represent an area quantity.");
	return x >= square_feet_t(80.0);
}
```

See the `units::traits` namespace for a list of all the supported traits.

# Changing the underlying type of `unit_t`

The default underlying type for all unit containers is `double`. However, this can be overridden by providing a definition for `UNIT_LIB_DEFAULT_TYPE`, e.g.

```cpp
// Use 64-bit integers as the underlying unit type
#define UNIT_LIB_DEFAULT_TYPE int64_t
#include <units.h>
```

**_NOTE:_ changing the underlying type may result in unexpected behavior.** Unit conversion makes heavy use of division, which may make integral types unsuitable except for niche embedded applications. Using excessively large types may increase the number of arithmetic overflow errors.

# Disabling IOStream

For some embedded applications, it may be [desirable to remove all references to `<iostream>` in order to reduce compiled binary size and RAM requirements](https://github.com/nholthaus/units/issues/32). There are two ways to accomplish this:

1. If you are copy/pasting `units.h` into your project include directory, then simply define `UNIT_LIB_DISABLE_IOSTREAM` before including the header.

   ```cpp
   #define UNIT_LIB_DISABLE_IOSTREAM
   #include <units.h>
   ```

2. If you are including `units` in your project as a `CMake` target (using `add_subdirectory`), then all you need to do is set the `DISABLE_IOSTREAM` cache option, either using the cmake-gui, or by adding the option to the cmake command line during configuration:

   ```bash
   cmake -DDISABLE_IOSTREAM=ON -DBUILD_TESTS=OFF ..
   cmake --build . --config Release
   ```

# Enabling a subset of units to improve compilation time

If you know that you only need a subset of the unit namespaces for your application, you can dramatically improve compilation time by disabling the default definitions, and then only opting-in to the namespaces you want. For example:

  ```cpp
  // Only use length and time
  #define DISABLE_PREDEFINED_UNITS
  #define ENABLE_PREDEFINED_LENGTH_UNITS
  #define ENABLE_PREDEFINED_TIME_UNITS
  ```

The generic algorithm is
  1. disable the pre-defined units using `#define DISABLE_PREDEFINED_UNITS`
  2. opt-in to the namespaces you want using `#define ENABLE_PREDEFINED_<namepsace name>_UNITS`

Additionally, for `CMake` users, there are equivalently-named cmake options defined which will automatically include the preprocessor definitions in your project.

# Macro clashes

With certain compilers, it is possible that system header files like `<ctype.h>` will define macros which conflict with the unit literals, which use SI abbreviations. In these cases, it is general safe and advisable to `#undef` the offending macros.

## Windows macros

`_T` is known to conflict, but is hardcoded into the compiler and can't be disabled. For this reason, `Tesla` units use the `_Te` abbreviation.

The following macros may need to be undefined on the Windows platform to use `units`:

   ```cpp
   #undef pascal
   #include <units.h>
   ```
   
## ARM macros

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

There are several ways to incorporate `units.h` into your project. The simplest is to just copy `include/units.h` into your project include directory (which the licensing allows you to do). However, you'll have to properly set up the necessary compilation flags for C++14 (`-std=c++14` on gcc).

However, if you are already using CMake as your build system, the recommended way to include `units` is to copy the entire `units` project as a subdirectory within your own top-level project folder. Then, in your CMakeLists.txt file add

   ```cmake
   add_subdirectory(units)

   add_executable(${PROJECT_NAME} main.cpp)
   target_link_libraries(${PROJECT_NAME} units)
   ```

Also, if you are distributing headers that depends on units.h, you shoud consider using cmake's `find_package` to check if the header is installed on the user's system:
    
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

The library itself consists of a single header [units.h](include/units.h), and can be included into your project without being built. 

The unit tests and documentation can be built with CMake. A doxygen installation is required to generate the documentation, and a Tex install is needed if pdf documentation is desired.

To build the tests:

## Windows
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
 1. Ensure you are using cmake 3.2 or later. You can verify this with `cmake --version`.
 2. Ensure you are using gcc version 4.9 or greater. You can verify this with `gcc --version`.
 3. clone the repository or download the `.tar.gz` package.
 4. Open a terminal and navigate to the source directory.
 5. Type the following commands:
   - `mkdir build`
   - `cd build`
   - `cmake -Wno-dev ..`
   - `cmake --build . --config Release`
 6. The tests will be created in an executable called `unitLibTest` in the folder `build/unitTests`.

# Previous Releases

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
