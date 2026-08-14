# The namespace map

*Where each name lives, and the two places you have to be deliberate: bringing in the literal operators,
and disambiguating a unit name that two dimensions share.*

Everything in the library lives under the top-level namespace `units`. A handful of nested namespaces
organize the pieces; most of the time you interact only with `units` itself and never type a nested
name. This page lays out the map and flags the exceptions.

## `units` — the main namespace

`units` is where you spend nearly all of your time. It contains:

- **The unit types.** `units::meters`, `units::seconds`, `units::newtons`, and every other quantity type.

- **The math functions.** `sqrt`, `pow`, `hypot`, the trigonometric functions, and the rest are declared
  directly in `units`. You call them **unqualified** — argument-dependent lookup finds them from the
  argument's type. Write `sqrt(area)`, not `units::sqrt(area)`.

  > **Note:** there is no `units::math` namespace. The math functions were moved to `units` proper so
  > that ADL finds them. If you have seen `units::math::sqrt` in older code or examples, drop the `math`.

Dimensions are **inline namespaces** inside `units`. Each dimension — length, mass, time, force, and so
on — is its own namespace (`units::length`, `units::mass`, …), but because it is *inline*, its contents
are also visible directly at the `units` level. That is why `units::meters` resolves even though the
canonical definition is `units::length::meters`:

```cpp
#include <units.h>
#include <type_traits>

static_assert(std::is_same_v<units::meters<double>, units::length::meters<double>>);
```

Both names denote the identical type. You almost never need the `units::length::` form; it exists for the
one case in the next section.

Physical constants and unit constants also live at the `units` level (in the inline `units::constants`
namespace — see below), so `units::constants::pi` and the `units::m` one-unit constant are reachable
without a nested qualifier.

## `units::literals` — the literal operators

The user-defined literal suffixes — `_m`, `_s`, `_kg`, and the rest — live in `units::literals`. Unlike
the dimension namespaces, `units::literals` is **not** inline, so its operators are not visible until you
introduce them with a `using`-directive:

```cpp
#include <units.h>
using namespace units::literals;   // required to use the _m / _s / ... suffixes

auto d = 5.0_m;    // meters<double>
auto t = 3_s;      // seconds<int>
```

Without the `using namespace units::literals;` line, `5.0_m` does not compile — the suffix operator is
out of scope.

> **Note:** the literal namespace is kept non-inline on purpose. Literal suffixes populate the global
> operator set, and a library should not force short suffixes like `_m` on every translation unit that
> merely includes a header. Opting in with a `using`-directive, at the narrowest scope you need it, keeps
> that choice yours.

> **Caveat:** a literal's representation follows the literal's form. `5_m` is `meters<int>`; `5.0_m` is
> `meters<double>`. Integer-backed quantities do integer arithmetic, so prefer the floating-point form
> unless you specifically want integer semantics. See [CTAD and ADL](ctad-and-adl-for-humans.md).

## `units::constants` — physical constants

`units::constants` holds the physical constants — the speed of light `c`, the gravitational constant
`G`, Planck's constant `h`, Avogadro's number `N_A`, `pi`, and the rest — each as a typed quantity in its
proper dimension. It is an inline namespace, so the constants are reachable through `units::` as well,
but qualifying them as `units::constants::…` is the clearest form and avoids a name that another
dimension happens to reuse:

```cpp
#include <units.h>

auto light = units::constants::c;    // meters_per_second — speed of light
auto ratio = units::constants::pi;   // dimensionless
```

> **Caveat:** because the constants are inline, a bare `units::c` can collide with an unrelated unit that
> uses the same short symbol (for example the volume unit `cups`, whose abbreviation is `c`). Reach for a
> constant through `units::constants::` to keep the reference unambiguous.

## `units::traits` — the type traits

The compile-time type traits live in `units::traits`. These are the predicates and accessors you use in
generic code and `static_assert`s — for example `units::traits::is_unit_v` to test whether a type is a
quantity, and the family of same-dimension and unit-inspection traits:

```cpp
#include <units.h>

static_assert(units::traits::is_unit_v<units::meters<double>>);
```

You need `units::traits` only when writing templates or constraints over quantities; ordinary
quantity-using code never mentions it.

## Dimension sub-namespaces — disambiguating shared names

The inline dimension namespaces (`units::length`, `units::mass`, `units::force`, …) do one job you cannot
do any other way: they distinguish two units that share a name across different dimensions.

Because every dimension is inlined into `units`, a name defined in exactly one dimension is reachable
bare (`units::meters`). But some names appear in more than one dimension — most notably **pounds**, which
is both a unit of mass and a unit of force:

```cpp
#include <units.h>

using MassLb  = units::mass::pounds<double>;    // pound (mass)
using ForceLb = units::force::pounds<double>;   // pound-force
```

These are different types with different dimensions. A bare `units::pounds` is **ambiguous** and will not
compile — the two inline definitions collide, and the compiler cannot choose. The sub-namespace is how
you say which one you mean:

```cpp
// units::pounds<double> p;         // error: ambiguous — mass or force?
units::mass::pounds<double>  m;     // unambiguous: mass
units::force::pounds<double> f;     // unambiguous: force
```

This is the entire reason the dimension sub-namespaces are worth knowing about. For every name that lives
in a single dimension, use the short `units::` form; reach for `units::<dimension>::` only when a name is
shared and the compiler tells you it is ambiguous.

---

See also: [CTAD and ADL](ctad-and-adl-for-humans.md) · [why use a units library](why-units.md) ·
[efficiency](efficiency.md) · [the type traits](../reference/type-traits.md) ·
[the cheat sheet](../reference/cheat-sheet.md)
