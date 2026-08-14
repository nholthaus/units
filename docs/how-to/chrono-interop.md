# std::chrono interoperability

Time quantities convert implicitly to and from `std::chrono::duration` in both directions.

A `units` time quantity and a `std::chrono::duration` model the same thing, and they interconvert: assign
a `std::chrono::duration` to a time quantity, or a time quantity to a `std::chrono::duration`, and the
value converts. This is enabled only for time units, so no other dimension can be silently mistaken for a
duration.

Related how-to guides: [defining new units](defining-new-units.md), [math functions](math-functions.md),
[JSON serialization](json-serialization.md).

## Both directions are implicit

```cpp
#include <units/time.h>
#include <chrono>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

    // std::chrono::duration -> units time quantity (the value converts across the ratio)
    units::time::seconds<double> s = std::chrono::seconds{5};    // 5 s
    units::time::minutes<double> m = std::chrono::seconds{90};   // 1.5 min

    // units time quantity -> std::chrono::duration
    std::chrono::duration<double> d = 5.0_s;                     // 5 s

    std::cout << s.value() << " s, " << m.value() << " min, " << d.count() << " s\n";
    // prints: 5 s, 1.5 min, 5 s
}
```

The chrono-to-units direction converts the duration's `Rep`/`Period` into the target unit; assigning
`std::chrono::seconds{90}` to a `minutes<double>` gives `1.5`. The units-to-chrono direction reads the
quantity's value into the duration's representation.

## Mixing in one expression

Because the chrono-to-units conversion is implicit, a `std::chrono::duration` participates anywhere a time
quantity is expected — including as an operand you first bind to a time quantity, and as a function argument.
Once both operands are time quantities, the arithmetic deduces its result unit as usual:

```cpp
using namespace units;
using namespace units::literals;

units::time::seconds<double> from_chrono = std::chrono::seconds{30};   // 0.5 min worth
units::time::minutes<double> total       = 1.5_min + from_chrono;      // 2 min
```

A function that takes a time quantity accepts a `std::chrono::duration` directly, since the conversion happens
at the call boundary:

```cpp
units::time::seconds<double> timeout_plus_grace(units::time::seconds<double> t)
{
    using namespace units::literals;
    return t + 5.0_s;
}

auto result = timeout_plus_grace(std::chrono::seconds{10});   // 15 s
```

Caveat: the built-in arithmetic operators require *both* operands to be units quantities. A raw
`std::chrono::duration` on one side of `+` (for example `1.5_min + std::chrono::seconds{30}`) does not compile,
because the operator's constraints reject a non-units operand rather than triggering the conversion mid-overload
resolution. Convert the chrono value to a time quantity first — assign it to a named variable, or pass it where
a quantity is expected — as shown above.

Note: `std::common_type` is specialized for the unnamed `unit<...>` time form and a `std::chrono::duration`, so
generic code that computes a common type over the base unit form is supported. A *named* time type such as
`minutes<double>` does not itself match that specialization; prefer an explicit conversion in mixed expressions.

## Deducing a quantity from a duration (CTAD)

A class template argument deduction guide constructs a time quantity directly from a `std::chrono::duration`,
deducing the unit from the duration's period:

```cpp
using namespace units;

units::unit q{std::chrono::milliseconds{250}};   // deduced as a millisecond-period time quantity
std::cout << q.value() << '\n';                    // prints: 250
```

The deduced unit carries the duration's period, so `q.value()` reports `250` (milliseconds), not `0.25`
(seconds). Assign to a named time type when you want the value in a specific unit.
