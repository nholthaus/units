#!/usr/bin/env python3
"""Drift check for units::detail::builtin_dimensions.

The visit() default candidate set (`builtin_dimensions` in include/units/serialization.h) is a
hand-maintained tuple of every dimension the library defines. C++ cannot enumerate the `dimension`
namespace header-only, so this CI-only check keeps the list honest: it parses the dimensions declared in
include/units/core.h and fails if any is missing from (or extra in) builtin_dimensions.

This runs in CI only. Consuming the library needs nothing — the header is plain and complete; this script
just guarantees a maintainer who adds a dimension also lists it, so visit() can resolve it by default.

Usage:  python3 scripts/check_builtin_dimensions.py
Exit 0 if in sync; exit 1 (with a diff) if drifted.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CORE = ROOT / "include" / "units" / "core.h"
SERIALIZATION = ROOT / "include" / "units" / "serialization.h"

# `dimensionless` is deliberately excluded: it is the empty signature, which visit() handles specially and
# which must never be a candidate.
EXCLUDED = {"dimensionless"}


def dimensions_in_core() -> set[str]:
    """The dimension aliases declared inside the `namespace dimension { ... }` block of core.h."""
    text = CORE.read_text()
    # isolate the dimension namespace block
    start = re.search(r"\n\tnamespace dimension\n\t\{", text)
    end = re.search(r"\n\t\}\s*//\s*namespace dimension", text)
    if not start or not end:
        sys.exit("check_builtin_dimensions: could not locate the `namespace dimension` block in core.h")
    block = text[start.end():end.start()]
    names = set(re.findall(r"^\s*using\s+([a-z_]+)\s*=", block, re.MULTILINE))
    return names - EXCLUDED


def dimensions_in_list() -> set[str]:
    """The dimensions listed in the builtin_dimensions tuple in serialization.h."""
    text = SERIALIZATION.read_text()
    m = re.search(r"using builtin_dimensions\s*=\s*std::tuple<(.*?)>;", text, re.DOTALL)
    if not m:
        sys.exit("check_builtin_dimensions: could not locate builtin_dimensions in serialization.h")
    return set(re.findall(r"dimension::([a-z_]+)", m.group(1)))


def main() -> int:
    declared = dimensions_in_core()
    listed = dimensions_in_list()

    missing = sorted(declared - listed)  # a dimension exists but visit() won't resolve it by default
    extra = sorted(listed - declared)    # listed but no longer a real dimension (rename/removal)

    if not missing and not extra:
        print(f"builtin_dimensions is in sync ({len(listed)} dimensions).")
        return 0

    print("builtin_dimensions has DRIFTED from the dimensions declared in core.h:\n")
    if missing:
        print("  MISSING from builtin_dimensions (add `dimension::<name>` to the tuple in serialization.h):")
        for name in missing:
            print(f"    - dimension::{name}")
    if extra:
        print("  EXTRA in builtin_dimensions (no longer declared in core.h; remove or rename):")
        for name in extra:
            print(f"    - dimension::{name}")
    print("\nUpdate include/units/serialization.h so visit() can resolve every dimension by default.")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
