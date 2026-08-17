vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO nholthaus/units
    REF v${VERSION}
    SHA512 0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
)

set(VCPKG_BUILD_TYPE "release")

# units is header-only. Turning tests off also turns the examples off (they follow UNITS_BUILD_TESTS), so the
# configure step compiles nothing and the install stages only the header tree and the CMake package files.
vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DUNITS_BUILD_TESTS=OFF
        -DUNITS_BUILD_EXAMPLES=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/units)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/lib") # header-only: no libraries, only the CMake config moved above

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
