# Packaging

`units` is a header-only library, so every package is just the header tree plus the CMake package
files — nothing is compiled to install it. This directory holds the reference sources for the
package-manager integrations, kept in sync with each release so the downstream recipes have a single
authoritative copy to track.

| Manager | Source here | Downstream home | Consume with |
|---------|-------------|-----------------|--------------|
| vcpkg   | [`vcpkg/`](vcpkg/) | [`microsoft/vcpkg` `ports/units`](https://github.com/microsoft/vcpkg/tree/master/ports/units) | `vcpkg install units` |
| Conan   | [`conan/`](conan/) | [`conan-io/conan-center-index` `recipes/units`](https://github.com/conan-io/conan-center-index/tree/master/recipes/units) | `conan install --requires=units/<version>` |
| Debian / Ubuntu (+ RPM, tarball) | [`../debian/`](../debian/) (and CPack in the root `CMakeLists.txt`) | Debian source package / the project PPA | `apt install libunits-dev` |

After `find_package(units CONFIG REQUIRED)`, link `units::units`. See
[`docs/how-to/cmake-integration.md`](../docs/how-to/cmake-integration.md).

## The one rule every package follows: compile nothing

The library's tests and examples are development artifacts, never part of a package. A packaging build
turns them off, so the configure step compiles nothing and the install stages only the headers and the
CMake config:

- `UNITS_BUILD_TESTS=OFF` — off for any package build.
- `UNITS_BUILD_EXAMPLES` follows `UNITS_BUILD_TESTS`, so turning tests off turns the examples off too;
  a package build need not name it, though the vcpkg port and `debian/rules` set it explicitly for clarity.
- `UNITS_INSTALL` is on when `units` is the top-level project (which every package build is), so the
  install/export rules run and lay down `unitsConfig.cmake`.

## vcpkg — [`vcpkg/`](vcpkg/)

`portfile.cmake` + `vcpkg.json`. Configures `units` directly, so it installs the full header tree and the
CMake package files with nothing compiled. On release, update `vcpkg.json`'s `version`, set `portfile.cmake`'s
`REF` to the new `v<version>` tag, and refresh the `SHA512` (the `SHA512` here is a zeroed placeholder that
vcpkg replaces during the downstream PR — `vcpkg install units` reports the expected hash on first fetch, or
run `vcpkg x-add-version`). The downstream update rides in a `[units] Update to <version>` PR against
`microsoft/vcpkg`.

## Conan — [`conan/`](conan/)

`conanfile.py` + `test_package/`. The recipe copies the whole `include/` tree (not just `units.h` — `units`
is a multi-header library), declares C++23 and the minimum compilers, and clears the package id (header-only).
`test_package/` is the consumer snippet Conan compiles to validate the package; it uses the current API
(`meters<double>`, `units::sqrt`). On release, add the version + source URL/`sha256` to `conandata.yml`
(supplied in the downstream `conan-center-index` PR) and bump `config.yml`. The recipe itself is
version-agnostic.

## Debian / RPM / tarball — [`../debian/`](../debian/) + CPack

The Debian packaging lives at the repository root in `debian/` (dpkg requires that location), producing
`libunits-dev`. `debian/rules` configures with `-DUNITS_BUILD_TESTS=OFF -DUNITS_BUILD_EXAMPLES=OFF`. The root
`CMakeLists.txt` also drives CPack (`DEB`, `RPM`, `TGZ`) for a `cpack`-based build. On release, add a
`debian/changelog` stanza for the new version; CPack takes its version from the CMake project version.
