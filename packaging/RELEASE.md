# Releasing a new version to the package managers

Every package here is version-pinned to the current release. Bumping to a new version is mechanical, but
two values can only be filled **after the `v<version>` git tag exists** (they are hashes of the tag's
archive tarball): the vcpkg `SHA512` and the Conan `sha256`. This file is the checklist for 3.6.0 and the
template for later releases.

Order of operations: merge the release prep to `main` → push the `v<version>` tag → then update these
recipes with the tarball hashes → open the downstream PRs.

## 3.6.0

### 1. Tag first

The recipes reference `https://github.com/nholthaus/units/archive/v3.6.0.tar.gz`, which does not exist
until the tag is pushed. Cut the tag on `main` after the 3.6.0 prep merges.

### 2. vcpkg — `vcpkg/`

- `vcpkg.json` `version` is `3.6.0`. ✓
- `portfile.cmake` `REF` is `v${VERSION}` (resolves to `v3.6.0`). ✓
- Fill `portfile.cmake` `SHA512` with the archive hash:
  ```
  curl -sL https://github.com/nholthaus/units/archive/v3.6.0.tar.gz | sha512sum
  ```
  (or run `./vcpkg install units --overlay-ports=packaging/vcpkg` once — vcpkg prints the expected SHA512
  on the hash mismatch, and `vcpkg x-add-version units` writes the version-database entry in the
  `microsoft/vcpkg` checkout.)
- Downstream PR: copy `portfile.cmake` + `vcpkg.json` into `ports/units/` of `microsoft/vcpkg`, run
  `vcpkg x-add-version units`, and open/refresh the `[units] Update to 3.6.0` PR (this supersedes the
  earlier draft PR #53446). The port compiles nothing (tests and examples off), so the libc++ platforms
  that failed the 3.5.1 draft build only when examples were compiled now install headers only — and the
  `is_unit` incomplete-type fix in 3.6.0 makes even an examples build clean on libc++.

### 3. Conan — `conan/`

- `conanfile.py` is version-agnostic (C++23, copies the whole header tree). ✓
- `config.yml` maps `3.6.0` → `all`. ✓
- Fill `conandata.yml` `sha256`:
  ```
  curl -sL https://github.com/nholthaus/units/archive/v3.6.0.tar.gz | sha256sum
  ```
- Validate locally before the PR:
  ```
  conan create packaging/conan --version 3.6.0
  ```
  (builds nothing for the package, then compiles `test_package/test_package.cpp` against the installed
  headers; it prints `5 m`).
- Downstream PR: the `conan-io/conan-center-index` `recipes/units` recipe is stale (pinned to 2.3.x,
  copies only `units.h`, uses the removed `meter_t` / `units::math` API). Replace `recipes/units/all/`
  with the files here and set `recipes/units/config.yml` to list `3.6.0`. Open the update PR.

### 4. Debian / RPM / tarball — `../debian/` + CPack

- `debian/changelog` has the `3.6.0` stanza. ✓
- CPack takes its version from the CMake project version (`3.6.0`). ✓
- Build with `dpkg-buildpackage` (PPA) or `cpack` from a top-level configure.
