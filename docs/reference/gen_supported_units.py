#!/usr/bin/env python3
"""
Generate docs/reference/supported-units.md from the unit definitions in include/units/*.h.

The catalog is derived from the headers so it can never drift from the code: re-run this whenever a
unit is added, removed, or renamed. Reads every UNIT_ADD* macro invocation, groups by dimension, and
emits a markdown table per dimension (unit type, literal, whether metric prefixes are provided).

Usage:
  python3 docs/reference/gen_supported_units.py > docs/reference/supported-units.md
"""
import re
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
INCLUDE = HERE.parent.parent / "include" / "units"

# Tokens that appear inside the UNIT_ADD macro *definitions* (in core.h) rather than real invocations.
SKIP = {"namePlural", "namespaceName", "abbreviation", "nameSingular"}

# UNIT_ADD(dim, plural, abbrev, ...) and its WITH_* variants share the (dim, plural, abbrev, ...) prefix.
UNIT_ADD = re.compile(
    r'\bUNIT_ADD(?:_WITH_METRIC_PREFIXES|_WITH_PLURAL_TAG|_WITH_METRIC_AND_BINARY_PREFIXES)?'
    r'\s*\(\s*(\w+)\s*,\s*(\w+)\s*,\s*(\w+)\s*,')
WITH_METRIC = re.compile(r'\bUNIT_ADD_WITH_METRIC_PREFIXES\s*\(\s*(\w+)\s*,\s*(\w+)')


def collect():
    """Return (rows, metric): rows[dim] = [(plural, abbrev), ...]; metric[dim] = {plural, ...}."""
    rows, metric = {}, {}
    for header in sorted(INCLUDE.glob("*.h")):
        if header.name == "core.h":
            continue
        text = header.read_text()
        for dim, plural, abbrev in (m.groups() for m in UNIT_ADD.finditer(text)):
            if plural in SKIP or abbrev in SKIP:
                continue
            rows.setdefault(header.stem, []).append((plural, abbrev))
        for dim, plural in (m.groups() for m in WITH_METRIC.finditer(text)):
            metric.setdefault(dim, set()).add(plural)
    return rows, metric


def main():
    rows, metric = collect()
    total = sum(len(v) for v in rows.values())
    out = []
    out.append("# Supported units")
    out.append("")
    out.append(f"*The catalog of built-in units, grouped by dimension — **{len(rows)} dimensions**, "
               f"**{total} named units** (before metric prefixes). Generated from the headers by "
               "`docs/reference/gen_supported_units.py`; do not edit by hand.*")
    out.append("")
    out.append("Each unit is available as a type (`meters`, `meters<double>`) and, where shown, a literal "
               "(`5.0_m`). Units marked **yes** under Prefixes also provide every SI metric prefix from "
               "femto to peta (e.g. `kilometers`/`_km`, `millimeters`/`_mm`). Include the umbrella header "
               "`<units.h>` for all of them, or a single `<units/DIMENSION.h>` for one dimension.")
    out.append("")
    out.append("For units shared across dimensions (e.g. `pounds` of mass vs. force), qualify with the "
               "dimension namespace: `units::mass::pounds` vs `units::force::pounds`.")
    out.append("")
    for dim in sorted(rows):
        out.append(f"## {dim.replace('_', ' ')}")
        out.append("")
        out.append("| Unit | Literal | Prefixes |")
        out.append("|------|---------|----------|")
        for plural, abbrev in rows[dim]:
            has_metric = "yes" if plural in metric.get(dim, ()) else ""
            out.append(f"| `{plural}` | `_{abbrev}` | {has_metric} |")
        out.append("")
    print("\n".join(out))


if __name__ == "__main__":
    main()
