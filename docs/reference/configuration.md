# Configuration

*Compile-time knobs that tailor the library: the preprocessor macros a consumer defines before including a header, and the CMake options that set them for you.*

The library is header-only and has no runtime configuration. Everything below is decided at compile time, either by a preprocessor macro you define before the first `#include <units.h>` (or `#include <units/…>`) or by a CMake `OPTION` that translates into one of those macros on the interface target.

> **Note:** A configuration macro must be visible **before** the library headers are first included in a translation unit, and it must be defined **consistently across the whole program**. Defining `UNIT_LIB_DEFAULT_TYPE` (or the iostream/literal switches) in one translation unit but not another produces distinct, incompatible instantiations and is an ODR violation. Prefer setting these once — a compiler `-D` flag or `target_compile_definitions` — rather than a bare `#define` in a single source file.

## Preprocessor macros

| Macro | Effect | Default | Where defined |
|---|---|---|---|
| `UNIT_LIB_DEFAULT_TYPE` | Library-wide default underlying (representation) type for a `unit` whose `T` argument is omitted, e.g. `meters<>`. | `double` (`core.h:50`) | `-D` / `target_compile_definitions` |
| `UNIT_NO_LITERAL_SUPPORT` | Suppresses generation of the `_x` user-defined literal operators (see [literals](literals.md)). | undefined (literals enabled) (`core.h:322`) | `-D` / `target_compile_definitions` |
| `UNIT_LIB_DISABLE_IOSTREAM` | Drops the `<iostream>`/`<clocale>` dependency and the stream (`operator<<`) support. | undefined (iostream enabled) (`core.h:71`) | `-D` / `target_compile_definitions`, or the `UNITS_DISABLE_IOSTREAM` CMake option |

### `UNIT_LIB_DEFAULT_TYPE`

Sets the representation type used when a unit template is instantiated without an explicit `T`. Change it to build a float-only or otherwise fixed-precision program — useful on embedded targets where `double` is expensive or unavailable.

```cpp
#define UNIT_LIB_DEFAULT_TYPE float
#include <units.h>
#include <type_traits>

int main()
{
    units::length::meters<> m(5.0f);   // underlying type is now float, not double
    static_assert(std::is_same_v<units::length::meters<>::underlying_type, float>);
    (void)m;
    return 0;
}
```

An explicit representation always overrides the default: `meters<double>` is `double` regardless of this macro. The macro only changes what `<>` (the omitted argument) resolves to.

### `UNIT_NO_LITERAL_SUPPORT`

Defines away the `UNIT_ADD_LITERALS` machinery so no `operator""_x` is emitted. Define it when user-defined literal operators would collide with another library, or to shrink a translation unit that never uses them.

```cpp
#define UNIT_NO_LITERAL_SUPPORT
#include <units.h>

int main()
{
    units::length::meters<double> m(5.0);   // constructors are unaffected
    // auto bad = 5.0_m;                     // error: no literal operator ""_m
    (void)m;
    return 0;
}
```

Every other way of constructing a quantity — the constructor, CTAD, and the `units::m`-style unit constants — continues to work; only the `_x` suffix is removed.

### `UNIT_LIB_DISABLE_IOSTREAM`

Removes the `<iostream>` and `<clocale>` includes and the `std::ostream& operator<<` overloads (`core.h:71`, `core.h:2865`, `core.h:4331`). This is the switch for embedded and freestanding builds that must not pull in `<iostream>`.

> **Note:** `name()` and `abbreviation()` are unaffected — they return `const char*` and do not depend on iostream. Only streaming a quantity to an `std::ostream` (and the formatting that entails) is removed.

```cpp
#define UNIT_LIB_DISABLE_IOSTREAM
#include <units.h>
#include <cstdio>

int main()
{
    units::length::meters<double> m(5.0);
    std::printf("%s / %s\n", m.name(), m.abbreviation());   // "meters / m" — still available
    // std::cout << m;   // error: operator<< is not compiled in this configuration
    return 0;
}
```

## CMake options

When the library is consumed via CMake (`add_subdirectory(units)` or `find_package(units CONFIG)`), the following `OPTION`s control the build (`CMakeLists.txt:12-15`). Their defaults depend on `MAIN_PROJECT` — `ON` when `units` is the top-level project (a standalone checkout), `OFF` when it is a subdirectory of a larger build.

| Option | Effect | Default |
|---|---|---|
| `UNITS_BUILD_TESTS` | Build the unit-test suite (pulls in GoogleTest). | `ON` as main project, else `OFF` |
| `UNITS_BUILD_DOCS` | Add the Doxygen `doc` target. | `OFF` |
| `UNITS_BUILD_EXAMPLES` | Compile (and run) the documentation examples so the guides' snippets cannot rot. | `ON` as main project, else `OFF` |
| `UNITS_DISABLE_IOSTREAM` | Define `UNIT_LIB_DISABLE_IOSTREAM` on the interface target (see above). | `OFF` |

`UNITS_DISABLE_IOSTREAM` is the only option that changes the *library's* behavior for consumers; it maps directly onto the `UNIT_LIB_DISABLE_IOSTREAM` compile definition (`CMakeLists.txt:43-45`). The remaining options affect only what the `units` project itself builds.

```cmake
# Consuming the library from a parent CMake project:
set(UNITS_DISABLE_IOSTREAM ON CACHE BOOL "" FORCE)   # embedded: no <iostream>
add_subdirectory(units)
target_link_libraries(my_app PRIVATE units::units)
```

To set `UNIT_LIB_DEFAULT_TYPE` or `UNIT_NO_LITERAL_SUPPORT` through CMake — there is no dedicated option for either — add the definition to your own target:

```cmake
target_compile_definitions(my_app PRIVATE UNIT_LIB_DEFAULT_TYPE=float)
target_compile_definitions(my_app PRIVATE UNIT_NO_LITERAL_SUPPORT)
```

The library requires C++23; the interface target already propagates `cxx_std_23` to consumers (`CMakeLists.txt:31`), so linking `units::units` is sufficient to enable it.

## See also

- [Literals](literals.md) — what `UNIT_NO_LITERAL_SUPPORT` turns off.
- [Cheat sheet](cheat-sheet.md) — the everyday API.
- [Type traits](type-traits.md) · [Concepts](concepts.md).
