# ---------------------------------------------------------------------------------------------------------------------
#  LLDB data formatter for the nholthaus `units` library (v3.x).
#
#  The companion to natvis/units.natvis (which serves the MSVC debugger): this makes every `units::unit<...>` — and
#  every named unit that derives from it (e.g. `units::length::meters<double>`) — print its magnitude and its unit
#  inline in LLDB (and LLDB-backed front-ends: CLion, Xcode, the VS Code CodeLLDB extension) instead of an opaque
#  object whose base classes you must expand. A 3-second value shows as:
#
#        (units::time::seconds<double>) t = 3 s
#
#  GENERAL BY DESIGN — nothing is hardcoded per unit. A single regex type-summary covers every unit type (length,
#  time, frequency, compound, dimensionless, decibel, ...) with no unit name enumerated anywhere.
#
#  USAGE — load it into LLDB (once per session, or from your ~/.lldbinit):
#        command script import /path/to/units/natvis/units_lldb.py
#  It registers a summary for the `units::unit<...>` regex; named units are matched too, since they derive from it.
#
#  HOW THE LABEL IS RESOLVED — two tiers, best-effort, mirroring the natvis:
#    1) `abbreviation()` is a constexpr member returning the true symbol ("s", "m", "Hz", "mps"). It is a function,
#       not stored data, so reading it needs debugger expression evaluation; when that is available we show the clean
#       abbreviation. Evaluation is attempted only on a live process (not a core dump) and is guarded so a failure
#       degrades rather than throws.
#    2) When evaluation is unavailable (optimized build, core file, or an abbreviation<> specialization that was
#       never instantiated) we FALL BACK to the conversion-factor tag type name parsed from the type, so the label is
#       always present and readable, never blank.
#
#  The magnitude is read directly from the `_linearized_value` member (no evaluation needed). If a future units
#  version renames `_linearized_value` or `abbreviation()`, update the anchors below.
# ---------------------------------------------------------------------------------------------------------------------

import re


# ---------------------------------------------------------------------------------------------------------------------
#      FUNCTION: _find_linearized_value
# ---------------------------------------------------------------------------------------------------------------------
#  Returns the SBValue of the `_linearized_value` member, searching the object and its base classes (a named unit
#  holds the value on its `units::unit<...>` base). Returns None if not found.
# ---------------------------------------------------------------------------------------------------------------------
def _find_linearized_value(valobj):
    direct = valobj.GetChildMemberWithName("_linearized_value")
    if direct and direct.IsValid():
        return direct
    # walk base classes / members for the value (named units derive from unit<...>)
    for i in range(valobj.GetNumChildren()):
        child = valobj.GetChildAtIndex(i)
        if not child or not child.IsValid():
            continue
        found = _find_linearized_value(child)
        if found and found.IsValid():
            return found
    return None


# ---------------------------------------------------------------------------------------------------------------------
#      FUNCTION: _tag_from_type_name
# ---------------------------------------------------------------------------------------------------------------------
#  Best-effort fallback label: the first template argument of `units::unit<...>` is the conversion-factor tag, whose
#  type name is always available. Returns a short, readable token (e.g. "meters_" -> "meters") or "unit" if it cannot
#  be parsed.
# ---------------------------------------------------------------------------------------------------------------------
def _tag_from_type_name(type_name):
    # peel to the innermost unit<...> template argument list
    match = re.search(r"units::unit<\s*([^,>]+)", type_name)
    token = match.group(1).strip() if match else type_name
    # strip a namespace qualification and a trailing conversion-factor underscore for readability
    token = token.split("::")[-1].strip()
    if token.endswith("_"):
        token = token[:-1]
    return token or "unit"


# ---------------------------------------------------------------------------------------------------------------------
#      FUNCTION: _abbreviation
# ---------------------------------------------------------------------------------------------------------------------
#  Best-effort true abbreviation via expression evaluation of `abbreviation()`. Returns the symbol string, or None if
#  evaluation is unavailable or yields nothing. Guarded so any failure returns None rather than raising.
# ---------------------------------------------------------------------------------------------------------------------
def _abbreviation(valobj):
    try:
        expr = "(const char*)(%s).abbreviation()" % valobj.GetName()
        result = valobj.GetFrame().EvaluateExpression(expr) if valobj.GetFrame().IsValid() else None
        if result and result.IsValid() and result.GetError().Success():
            summary = result.GetSummary()
            if summary:
                return summary.strip().strip('"') or None
    except Exception:
        pass
    return None


# ---------------------------------------------------------------------------------------------------------------------
#      FUNCTION: unit_summary
# ---------------------------------------------------------------------------------------------------------------------
#  The LLDB summary provider for any units quantity. Formats as "<magnitude> <label>", where label is the true
#  abbreviation when evaluable, else the conversion-factor tag name in brackets.
# ---------------------------------------------------------------------------------------------------------------------
def unit_summary(valobj, internal_dict):
    value = _find_linearized_value(valobj)
    if value is None or not value.IsValid():
        return ""
    magnitude = value.GetValue()
    if magnitude is None:
        return ""

    abbreviation = _abbreviation(valobj)
    if abbreviation:
        return "%s %s" % (magnitude, abbreviation)
    return "%s [%s]" % (magnitude, _tag_from_type_name(valobj.GetType().GetName()))


# ---------------------------------------------------------------------------------------------------------------------
#      FUNCTION: __lldb_init_module
# ---------------------------------------------------------------------------------------------------------------------
#  LLDB calls this on `command script import`. Registers the summary for the `units::unit<...>` type by regex; named
#  units that derive from it are covered because LLDB applies a base-class summary to a derived type.
# ---------------------------------------------------------------------------------------------------------------------
def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand(
        'type summary add -x "^units::unit<.+>$" -F units_lldb.unit_summary -w units --category-enable'
    )
    debugger.HandleCommand("type category enable units")
