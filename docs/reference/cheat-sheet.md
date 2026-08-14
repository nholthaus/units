# Cheat sheet

*The everyday API on one page. Every line here compiles under C++23. Assumes:*

```cpp
#include <units.h>
using namespace units;
using namespace units::literals;
```

## Make a quantity

```cpp
meters a(5.0);            // CTAD -> meters<double>
meters b = 5.0_m;         // literal
auto   c = 5.0 * m;       // scalar * unit constant (units::m)
meters d{5.0};            // braced
meters<float> e(5.0f);    // explicit representation
meters<int>   f(5);       // integer representation
// meters g;              // needs an explicit arg: meters<double> g;
```

Literal representation follows the decimal point: `5_m` is `meters<int>`, `5.0_m` is `meters<double>`.

## Convert (implicit, lossless only)

```cpp
meters m = 100.0_ft;      // feet -> meters
feet   f = m;             // meters -> feet
minutes t = 90.0_s;       // seconds -> minutes (value 1.5)
// meters<int> x = 1.0_ft; // ERROR: lossy into an integer representation
```

## Arithmetic (dimensions are tracked)

```cpp
square_meters     area  = 15.0_m * 5.0_m;   // m * m -> area
meters_per_second speed = 60.0_mi / 1.0_hr;  // -> velocity
auto              any   = 15.0_m * 5.0_m;    // auto: whatever it produces (square_meters)
// meters bad = 15.0_m * 5.0_m;              // ERROR: m*m is an area, not a length
// auto z = 1.0_m + 1.0_s;                   // ERROR: incompatible dimensions
// auto w = 1.0_m + 5.0;                     // ERROR: scalar + dimensioned quantity
```

## Get a plain number out

```cpp
double v = d.value();     // value in the quantity's units
double r = d.raw();        // stored value (differs from value() for percent/ppm)
double t = d.to<double>(); // explicit cast to a representation
int    i = d.to<int>();    // explicit, truncates toward zero
double u = unit_cast<double>(d);   // equivalent to .to<double>()
// double x = d;           // ERROR: no implicit unit -> double (dimensionless is the exception)
```

## Compare

```cpp
bool lt = (1.0_m <  2.0_ft);     // converts, then compares
bool eq = (1.0_m == 100.0_cm);   // true
// bool bad = (1.0_m < 1.0_kg);  // ERROR: incomparable dimensions
```

## Math (unqualified — found by ADL)

```cpp
meters h  = sqrt(pow<2>(3.0_m) + pow<2>(4.0_m));   // 5 m
meters mn = min(1.0_m, 2.0_ft);
meters mx = max(1.0_m, 2.0_ft);
meters ab = abs(-3.0_m);
auto   s  = sin(90.0_deg);        // trig needs an angle
// auto bad = sin(1.0_m);         // ERROR: sin needs an angle, not a length
auto   x  = units::hypot(3.0_m, 4.0_m);   // qualify if an unqualified call is ambiguous
```

## Name, print, serialize

```cpp
std::cout << 5.0_m;                 // "5 m"
std::string s = to_string(5.0_m);   // "5 m"
const char* n = (5.0_m).name();     // "meters"
const char* z = (5.0_m).abbreviation(); // "m"
```

## Dimensionless and percent

```cpp
dimensionless<double> ratio = 0.25;  // double -> dimensionless (implicit)
double back = ratio;                  // dimensionless -> double (implicit)
auto p = 50.0_pct;
double frac = p.value();   // 0.5  (the fraction)
double pts  = p.raw();     // 50   (the point count)
```

## chrono interop

```cpp
units::time::seconds<double> s = std::chrono::seconds{5};   // chrono -> unit
std::chrono::duration<double> d = 5.0_s;                    // unit -> chrono
```

## Special values

```cpp
auto nan = std::numeric_limits<meters<double>>::quiet_NaN();
auto inf = std::numeric_limits<meters<double>>::infinity();
bool b1 = isnan(nan);      // true   (unqualified, ADL)
bool b2 = isinf(inf);      // true
bool b3 = isfinite(5.0_m); // true
```

## Constrain your own templates (concepts)

```cpp
template <units::UnitType U>
U twice(U x) { return x + x; }   // accepts any quantity

template <units::DimensionlessUnitType U>
double as_number(U x) { return x.value(); }
```

## Define a unit (one line)

```cpp
namespace units {
    UNIT_ADD(length, smoots, smoot, conversion_factor<std::ratio<17018, 10000>, meters<>>)
}
auto bridge = 364.4_smoot;   // now a usable length, with its own literal
```

See [defining new units](../how-to/defining-new-units.md) for prefixes and compound units.

## Handy references

- [Supported units](supported-units.md) · [Constants](constants.md) · [Configuration macros](configuration.md)
- [Type traits](type-traits.md) · [Concepts](concepts.md)
- [Math functions](../how-to/math-functions.md) · [FAQ](../meta/faq.md)
