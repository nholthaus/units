# Disabling iostream

*For embedded or freestanding builds where `<iostream>` is unwanted, `units` can be compiled without it.
Define `UNIT_LIB_DISABLE_IOSTREAM` before including the library. The unit types, arithmetic, conversions,
and `name()`/`abbreviation()` all continue to work; only the stream/string rendering is removed.*

## What it does

Defining the macro drops the `operator<<` overloads, `to_string`, and the `<clocale>`/`<iostream>`
includes. Everything else is unaffected.

```cpp
#define UNIT_LIB_DISABLE_IOSTREAM
#include <units/length.h>

int main()
{
    using namespace units;
    using namespace units::literals;

    meters d = 5.0_m;
    double v = d.value();          // works
    const char* a = d.abbreviation();  // "m" — still available (it is a plain const char*)
    // std::cout << d;             // NOT available in this mode
    (void)v; (void)a;
}
```

`name()` and `abbreviation()` return `const char*` and do not depend on `<iostream>` or `<string>`, so
they remain available for producing a label without pulling in the streaming machinery.

## How to set it

**Per translation unit** — define the macro before the first `units` include:

```cpp
#define UNIT_LIB_DISABLE_IOSTREAM
#include <units.h>
```

**Project-wide via CMake** — set the `UNITS_DISABLE_IOSTREAM` option, which defines the macro on the
interface target for every consumer:

```cmake
set(UNITS_DISABLE_IOSTREAM ON)
add_subdirectory(units)
target_link_libraries(myapp PRIVATE units::units)
```

> **Note — embedded builds.** On platforms whose system headers define macros that clash with the SI
> abbreviations, also see the macro-clash guidance (`#undef pascal` on Windows; several single-letter
> `#undef`s on some ARM toolchains). Disabling iostream is commonly combined with those on embedded
> targets. The related configuration macros are listed in
> [configuration](../reference/configuration.md).
