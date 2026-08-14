#!/usr/bin/env python3
"""
Generate the reference tables (supported units, physical constants) from the headers, and keep the
copies embedded in README.md in sync so they can never drift from the code.

Everything is derived from include/units/*.h and include/units.h, so re-run this whenever a unit or
constant is added, removed, or renamed:

  python3 docs/reference/gen_reference.py            # rewrite files + inject README blocks
  python3 docs/reference/gen_reference.py --check    # exit 1 if anything is out of date (for CI)

It (1) rewrites docs/reference/supported-units.md, (2) injects the supported-units and constants tables
into README.md between the marker comments:

  <!-- BEGIN generated: supported-units -->  ...  <!-- END generated: supported-units -->
  <!-- BEGIN generated: constants -->        ...  <!-- END generated: constants -->
"""
import argparse
import re
import sys
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parent.parent
INCLUDE = ROOT / "include" / "units"
UNITS_H = ROOT / "include" / "units.h"
README = ROOT / "README.md"
SUPPORTED_MD = HERE / "supported-units.md"

# Tokens that appear inside the UNIT_ADD macro *definitions* (in core.h) rather than real invocations.
SKIP = {"namePlural", "namespaceName", "abbreviation", "nameSingular"}

UNIT_ADD = re.compile(
    r'\bUNIT_ADD(?:_WITH_METRIC_PREFIXES|_WITH_PLURAL_TAG|_WITH_METRIC_AND_BINARY_PREFIXES)?'
    r'\s*\(\s*(\w+)\s*,\s*(\w+)\s*,\s*(\w+)\s*,')
WITH_METRIC = re.compile(r'\bUNIT_ADD_WITH_METRIC_PREFIXES\s*\(\s*(\w+)\s*,\s*(\w+)')
# A physical constant: `... symbol(value);   ///< doc`
CONSTANT = re.compile(r'\b([A-Za-z_]\w*)\s*\(\s*([0-9.eE+\-]+)\s*\)\s*;\s*///<\s*(.+?)\s*$')


#----------------------------------------------------------------------------------------------------
#      collectors
#----------------------------------------------------------------------------------------------------
def collect_units():
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


def collect_constants():
    """Return [(symbol, value, doc), ...] in source order from the units::constants block in units.h."""
    text = UNITS_H.read_text()
    # Restrict to the `inline namespace constants { ... }` block so we don't pick up unrelated lines.
    start = text.find("namespace constants")
    block = text[start:] if start != -1 else text
    out = []
    for line in block.splitlines():
        m = CONSTANT.search(line)
        if m:
            out.append((m.group(1), m.group(2), m.group(3).rstrip(".")))
    return out


#----------------------------------------------------------------------------------------------------
#      renderers
#----------------------------------------------------------------------------------------------------
def render_units_tables(rows, metric):
    """The by-dimension unit tables (shared by the README block and the standalone reference page)."""
    out = []
    for dim in sorted(rows):
        out.append(f"### {dim.replace('_', ' ')}")
        out.append("")
        out.append("| Unit | Literal | Prefixes |")
        out.append("|------|---------|----------|")
        for plural, abbrev in rows[dim]:
            has_metric = "yes" if plural in metric.get(dim, ()) else ""
            out.append(f"| `{plural}` | `_{abbrev}` | {has_metric} |")
        out.append("")
    return "\n".join(out).rstrip()


def render_constants_table(constants):
    out = ["| Symbol | Constant | Value |", "|--------|----------|-------|"]
    for sym, val, doc in constants:
        out.append(f"| `{sym}` | {doc} | {val} |")
    return "\n".join(out)


def supported_units_page(rows, metric):
    total = sum(len(v) for v in rows.values())
    return (
        "# Supported units\n\n"
        f"*The catalog of built-in units, grouped by dimension — **{len(rows)} dimensions**, "
        f"**{total} named units** (before metric prefixes). Generated from the headers by "
        "`docs/reference/gen_reference.py`; do not edit by hand.*\n\n"
        "Each unit is available as a type (`meters`, `meters<double>`) and, where shown, a literal "
        "(`5.0_m`). Units marked **yes** under Prefixes also provide every SI metric prefix from femto "
        "to peta (e.g. `kilometers`/`_km`, `millimeters`/`_mm`). Include the umbrella header `<units.h>` "
        "for all of them, or a single `<units/DIMENSION.h>` for one dimension.\n\n"
        "For units shared across dimensions (e.g. `pounds` of mass vs. force), qualify with the dimension "
        "namespace: `units::mass::pounds` vs `units::force::pounds`.\n\n"
        + render_units_tables(rows, metric) + "\n"
    )


#----------------------------------------------------------------------------------------------------
#      README injection
#----------------------------------------------------------------------------------------------------
def inject(text, name, body):
    """Replace the content between the BEGIN/END markers for `name`; error if the markers are absent."""
    begin, end = f"<!-- BEGIN generated: {name} -->", f"<!-- END generated: {name} -->"
    pattern = re.compile(re.escape(begin) + r".*?" + re.escape(end), re.S)
    if not pattern.search(text):
        raise SystemExit(f"error: markers for '{name}' not found in README.md")
    return pattern.sub(begin + "\n" + body + "\n" + end, text)


def build_readme(text, rows, metric, constants):
    text = inject(text, "supported-units", render_units_tables(rows, metric))
    text = inject(text, "constants", render_constants_table(constants))
    return text


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true", help="exit 1 if any output is out of date")
    args = ap.parse_args()

    rows, metric = collect_units()
    constants = collect_constants()
    page = supported_units_page(rows, metric)
    readme_new = build_readme(README.read_text(), rows, metric, constants)

    if args.check:
        stale = []
        if SUPPORTED_MD.read_text() != page:
            stale.append(str(SUPPORTED_MD.relative_to(ROOT)))
        if README.read_text() != readme_new:
            stale.append("README.md")
        if stale:
            print("out of date (run docs/reference/gen_reference.py): " + ", ".join(stale))
            sys.exit(1)
        print("reference tables are up to date")
        return

    SUPPORTED_MD.write_text(page)
    README.write_text(readme_new)
    print(f"wrote {SUPPORTED_MD.relative_to(ROOT)} and injected README.md "
          f"({len(rows)} dimensions, {sum(len(v) for v in rows.values())} units, {len(constants)} constants)")


if __name__ == "__main__":
    main()
