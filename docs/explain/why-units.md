# Why use a units library

*A quantity is a number and a unit. This library makes the unit part of the type, so the compiler
enforces it — and produces no code to do so.*

A physical quantity carries two pieces of information: a magnitude and a unit. Conventional programs
represent only the first. A `double distance` stores `1500.0`; whether that is meters, feet, or
nautical miles lives in a comment, a variable name, or the programmer's memory. Nothing in the type
system distinguishes a distance from a duration, a distance in meters from the same distance in feet,
or a valid assignment from a silent unit mismatch.

`units` closes that gap. A quantity such as `meters<double>` is a distinct type; its unit is a
template parameter, checked at compile time and erased before code generation. Mixing incompatible
units is a compilation error, not a wrong answer at run time.

## The class of bug it eliminates

The canonical failure is the loss of the Mars Climate Orbiter in 1999. One software component produced
impulse values in pound-force seconds; the component consuming them expected newton seconds. Both were
`double`. The types agreed, the code compiled, the numbers flowed, and the spacecraft's trajectory was
off by the ratio between the two units — roughly a factor of 4.45. No tool in the build could see the
error, because to the compiler there was no error: two `double`s were being passed around.

This is the general shape of the *unit-mismatch bug*:

- A value is produced in one unit and consumed as if it were another.
- Both sides are the same underlying arithmetic type, so the type checker is silent.
- The defect surfaces only as a wrong result — often far from its cause, sometimes only under specific
  inputs, sometimes not until the artifact is in the field.

Related members of the same family: adding a length to a time; assigning an area to a length variable;
passing an angle in degrees to a routine that expects radians; forgetting a conversion entirely.

With typed quantities, each of these is rejected where it is written:

```cpp
#include <units.h>
using namespace units::literals;

units::meters<double>  length  = 5.0_m;
units::seconds<double> elapsed = 3.0_s;

auto ok  = length + 2.0_m;   // meters<double> — fine, same dimension
// auto no = length + elapsed;  // does not compile: length + time is meaningless
```

The `length + elapsed` line is not a run-time check that throws; it is a type error the compiler
reports before the program is ever run. The mismatch cannot reach a test, a review, or a mission.

## Compile-time checking, not run-time checking

*When* the enforcement happens determines what it costs and what it can catch.

Run-time unit checking — tagging each value with a unit identifier and comparing tags during
arithmetic — is possible, but it pays for every operation with a branch and a comparison, and it can
only report a mismatch once execution reaches the offending line with the offending data. A path that a
test never exercises is a path never checked.

`units` performs all dimensional analysis during compilation. The unit of every intermediate result is
computed from the units of its operands by the type system: `meters / seconds` *is* a velocity type,
`meters * meters` *is* an area type, and there is no representation in which a length and a time can be
added. Because the check is structural, it covers every path through the code whether or not a test
runs it, and because it is resolved at compile time, none of it survives into the emitted program.

> **Note:** compile-time checking catches *dimensional* errors — mixing units that do not belong
> together. It does not catch a *magnitude* error you write within a single unit (storing `9.8` where you
> meant `98.0` in the same unit is still a plain numeric mistake). What it removes is the entire
> conversion-and-mismatch category, which is where the expensive, hard-to-find defects live.

## A typed quantity versus "a `double` plus a naming convention"

The informal alternative is discipline: name the variable `distance_m`, document the unit in the API,
review carefully. This holds only as well as the least careful change to touch the code, and it degrades
under the conditions where correctness matters most — a large codebase, many contributors, a refactor
that renames one side but not the other, an interface boundary where the convention on each side was
decided independently.

A naming convention has three structural weaknesses that a type does not:

1. **It is not enforced.** `distance_m = velocity_fps * time_s` compiles. The suffixes are decoration;
   the compiler reads only the `double`s.
2. **It does not compose.** When you multiply two conventionally-named values, the result has no name —
   and no convention tells you the unit of `a_m * b_s`. A typed quantity's result type is computed for
   you: `meters * seconds` yields the correct compound type automatically.
3. **It does not convert.** A convention tells you a value is in feet; it does not turn it into meters.
   You still write the conversion by hand, which is exactly the step the Orbiter's authors omitted.

A typed quantity makes the unit a property the compiler tracks, propagates through arithmetic, and
converts implicitly and correctly when — and only when — the dimensions match:

```cpp
#include <units.h>
using namespace units::literals;

units::meters<double> d = 100.0_ft;   // implicit, correct conversion: d.value() == 30.48
units::feet<double>   f = d;          // and back again, exact
```

The conversion factor is applied by the library, from a single authoritative definition, in one place —
not re-derived and re-typed at every call site where a human might get it wrong.

## No run-time cost

Wrapping every number in a class, tracking units, and inserting conversions does not slow the program
down. The safety is a property of the *types*, and types do not exist at run time.

- A quantity is a trivially copyable value the size of its underlying representation. `meters<double>`
  is the size of a `double` and is copied like one.
- Conversions are compile-time rational arithmetic. Converting feet to meters multiplies by a ratio the
  compiler already knows; a conversion between two spellings of the same unit emits no code at all.
- The dimensional bookkeeping happens entirely in the type system and is gone by the time the optimizer
  runs.

The result is that an optimized build of code using `units` produces the same machine code as the
equivalent hand-written `double` arithmetic — with the mismatches removed. The full argument, with a
worked example of a multi-step conversion collapsing to a single multiply, is in
[efficiency](efficiency.md).

## When not to use it

A units library is the right tool for code that manipulates physical quantities. It is not a universal
wrapper for every number, and a few situations argue against it:

- **Values with no dimension and no unit discipline to enforce** — array indices, counts, opaque
  identifiers, bit flags. There is nothing to check; a plain integer is clearer.
- **A hard constexpr requirement across the transcendental functions.** Most of the API is `constexpr`,
  including conversions and the algebraic math (`sqrt`, `pow`). The `<cmath>`-backed wrappers (the
  trigonometric functions, `exp`, `log`) are *not* usable in a constant expression, because the standard
  library functions they call are not. If a value must be computed at compile time through, say, a
  cosine, that specific path is unavailable. See [efficiency](efficiency.md) for the exact boundary.
- **Interop layers that must speak in raw numbers** — a C API, a serialization format, a hardware
  register. Here you convert to a plain value at the boundary (`.value()`, `.raw()`, or `.to<T>()`) and
  keep typed quantities on your side of it. The boundary is where the conversion is explicit and
  localized.

Outside those cases, the cost is a template dependency and the syntax to learn, against the elimination
of an entire bug class at no run-time expense.

---

See also: [efficiency](efficiency.md) · [the namespace map](namespaces.md) ·
[type safety](type-safety.md) · [the cheat sheet](../reference/cheat-sheet.md)
