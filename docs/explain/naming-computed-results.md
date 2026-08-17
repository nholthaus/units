# Naming computed results consistently

When you multiply or divide quantities you get a new kind: `meters / seconds` is a velocity,
`mass * acceleration` is a force. The friendly name for that result — `meters_per_second`, `newtons` —
lives in the result's dimension header (`<units/velocity.h>`, `<units/force.h>`). Two small habits keep
computed results naming and dispatching the same way throughout a program.

## Include the header your result lands in

If a translation unit names or prints a computed result, include that result's dimension header, not just
the headers of the operands. Dividing `meters` by `seconds` yields a velocity, so a file that prints or
names that result reads best with `<units/velocity.h>` in scope:

```cpp
#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>   // the result lands here

auto v = meters<double>(10) / seconds<double>(2);   // names and prints as meters_per_second
```

This is a best practice, not a requirement — the value and the dimension are always correct either way.
With the header, the result carries its friendly name (`5 mps`) and diagnostics read `meters_per_second`;
without it, the same value prints in dimension form (`5 m s^-1`) and diagnostics spell out the underlying
type.

## Dispatch on the dimension concept

Because a result's friendly name comes from its dimension header, whether a computed value matches an
overload or specialization written for the *concrete named type* (`meters_per_second`) depends on whether
that header was in scope. A function keyed on the concrete type can match the same computed value in one
file and miss it in another that included a different set of headers.

Constrain on the **dimension concept** instead — `units::Velocity`, `units::Force`, `units::Length`, and
one for every dimension. A concept classifies by dimension, so it matches a result the same way everywhere,
independent of the header set:

```cpp
void handle(units::Velocity auto v);   // matches any velocity, in any translation unit

template <units::Force F>
F clamp_force(F f);
```

The concept is the robust choice for any function, trait, or overload that dispatches on a computed
quantity. Every dimension has one, generated alongside its `traits::is_<dimension>_unit` trait — see the
[concepts reference](../reference/concepts.md).
