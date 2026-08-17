# CMake integration

*How to consume `units` from a CMake project. It is header-only, so there is nothing to build — you only
need its include directory on your target, which every method below arranges. The exported target is
`units::units`.*

## Copy the headers (no CMake)

Put `include/` on your compiler's include path and build with C++23 (`-std=c++23` on GCC and Clang,
`/std:c++latest` on MSVC). Nothing else is required.

## `add_subdirectory`

Vendor the project (a submodule or a copy) and add it:

```cmake
add_subdirectory(units)
target_link_libraries(myapp PRIVATE units::units)
```

Linking the target propagates the include directory and the C++23 requirement, so you do not set either
yourself.

## `FetchContent`

Pull it at configure time — no vendored copy in your tree:

```cmake
include(FetchContent)
FetchContent_Declare(units
  GIT_REPOSITORY https://github.com/nholthaus/units.git
  GIT_TAG        v3.6.0)          # pin a release tag
FetchContent_MakeAvailable(units)

target_link_libraries(myapp PRIVATE units::units)
```

> **Note — a nested consumer build is inert.** When `units` is not the top-level project it builds
> nothing of its own: no tests, no examples, no documentation, and it pulls in no CTest scaffolding (your
> cache gets no `BUILD_TESTING` option and no `Nightly`/`Continuous`/`Experimental` targets from `units`).
> Linking `units::units` is header-only, so all you get is the include path and `cxx_std_23`. If you want to
> be explicit, set the options before `MakeAvailable`/`add_subdirectory`: `set(UNITS_BUILD_TESTS OFF)` and
> `set(UNITS_BUILD_EXAMPLES OFF)`.
>
> A nested `units` also generates **no install/export rules**, so your application's `cmake --install` does
> not stage `units`' headers or CMake package files into your install tree. A packager that vendors `units`
> as a subdirectory and *does* want it installed can force the rules on with `set(UNITS_INSTALL ON)` before
> `add_subdirectory`. When `units` is the top-level project (a CPack build, or a Conan/vcpkg recipe that
> configures `units` directly), the install rules are on by default.

## Installed package: `find_package`

If `units` is installed (via `cmake --install`, or a Linux package), consume it with `find_package`:

```cmake
find_package(units CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE units::units)
```

The installed package exports `unitsConfig.cmake` / `unitsConfigVersion.cmake` / `unitsTargets.cmake`
with `SameMajorVersion` compatibility, so `find_package(units 3 REQUIRED)` accepts any 3.x.

To install from source:

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/desired/prefix
cmake --build build --target install
```

## Linux packages

The project's CPack configuration produces a Debian package (`libunits-dev`), an RPM (`units-devel`), and
a tarball; a PPA is published for Ubuntu. Installing any of these places the headers and the CMake package
files so `find_package(units CONFIG REQUIRED)` works system-wide.

> **Not yet available:** vcpkg and Conan ports. Contributions welcome.

## What linking brings

Linking `units::units` propagates the include path and `cxx_std_23`. On MSVC it also attaches the
[debugger visualizer](natvis.md), so a quantity displays as `5 m` in the debugger. See
[configuration](../reference/configuration.md) for the build-time options (`UNITS_DISABLE_IOSTREAM`, …).
