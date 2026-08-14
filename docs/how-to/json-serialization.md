# JSON serialization

Opt-in `nlohmann/json` support: quantities serialize to and from JSON when `<nlohmann/json.hpp>` is visible
before `units`.

The library integrates with [nlohmann/json](https://github.com/nlohmann/json) without taking a dependency on
it. If your translation unit includes `<nlohmann/json.hpp>` before it includes a `units` header, the library
defines `to_json`/`from_json` hooks so any quantity converts to and from a `nlohmann::json` value. If
nlohmann/json is not present, the hooks are absent and the library is unaffected.

Related how-to guides: [defining new units](defining-new-units.md), [math functions](math-functions.md),
[chrono interop](chrono-interop.md).

## The activation contract

At the bottom of `core.h` the JSON hooks are guarded by `__has_include`:

```cpp
#if defined __has_include
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
// ... to_json / from_json defined here ...
#endif
#endif
```

The consequence is a strict include-order requirement: **include `<nlohmann/json.hpp>` before any `units`
header.** The `__has_include` check runs while `units` is being compiled; the include-guard on nlohmann's
header means that if you have already included it, the units-side `#include <nlohmann/json.hpp>` is a no-op and
the hooks are defined. Include units first and nlohmann second, and the check may not see the header when it
matters.

```cpp
#include <nlohmann/json.hpp>   // MUST precede units for the hooks to activate
#include <units/length.h>
```

Note: there is no build-system dependency and no CMake option to toggle this. The feature is purely a
compile-time, header-presence opt-in — projects that never include nlohmann/json compile exactly as before.

## Round-trip

`to_json` writes the quantity's raw magnitude; `from_json` reconstructs the quantity from a numeric JSON value.

```cpp
#include <nlohmann/json.hpp>
#include <units/length.h>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

    length::meters<double> distance = 42.0_m;

    nlohmann::json j = distance;                       // to_json
    std::cout << j.dump() << '\n';                     // prints: 42.0

    auto back = j.get<length::meters<double>>();       // from_json
    std::cout << back.value() << " m\n";               // prints: 42 m
}
```

Quantities nest inside objects and arrays like any other JSON value:

```cpp
nlohmann::json obj = { {"distance", distance}, {"label", "leg-1"} };
std::cout << obj.dump() << '\n';                       // {"distance":42.0,"label":"leg-1"}

auto d = obj["distance"].get<length::meters<double>>();
std::cout << d.value() << " m\n";                      // prints: 42 m
```

Caveat: the JSON encoding is the bare magnitude — the unit is *not* stored in the JSON. `to_json` serializes
`u.raw()`, and `from_json` reads a number and constructs the target type from it. The unit is fixed by the C++
type you deserialize into (`j.get<meters<double>>()` yields meters; `j.get<feet<double>>()` would read the same
number as feet). Both peers must agree on the unit out of band; JSON alone does not disambiguate it.

Note: this integration is currently exercised only through the guarded `examples/json_roundtrip.cpp` and is not
covered by the unit-test suite. The behavior above is documented and compiles as shown, but treat it as a
lightly-tested convenience rather than a hardened, spec-guaranteed surface.
