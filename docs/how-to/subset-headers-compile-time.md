# Including a subset for faster compiles

*The umbrella header `<units.h>` pulls in every dimension. If a translation unit needs only a few, include
the per-dimension headers instead — you pay compile time only for what you use.*

## The two ways to include

**Everything** — one include, heavier:

```cpp
#include <units.h>   // all 47 dimensions, plus the physical constants
```

**Only what you need** — lighter. Each dimension has its own header under `units/`:

```cpp
#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>   // needed if you name meters_per_second explicitly
```

A per-dimension header is self-contained (it includes the core machinery it needs), so any of them
compiles on its own.

> **Note — pull in the dimensions your *results* land in, not just your operands.** Dividing a `meters`
> by a `seconds` yields a velocity, so if you name that result `meters_per_second` you need
> `<units/velocity.h>` even though you only wrote lengths and times. If you use `auto` for the result you
> still need the header for the dimension the value belongs to when you later convert or print it. When in
> doubt, include the dimension of every quantity you name.

## Effect on compile time

The library is heavily templated, so translation-unit compile time scales with how much of it you
instantiate. Restricting the includes to the dimensions a file actually uses keeps that file's compile
time down; it does not change run-time behavior or code size (unused templates are never instantiated).
For the broader performance picture, see [efficiency](../explain/efficiency.md).

> **Caveat — the constants live in the umbrella header.** `units::constants` (`c`, `G`, `h`, …) is
> defined in `<units.h>`. If you need a physical constant, include the umbrella header; the per-dimension
> headers do not provide the constants.
