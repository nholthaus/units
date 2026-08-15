# Debugger visualizers

*`units` ships debugger visualizers so that a quantity displays as its value and abbreviation — `5 m`,
`3 s` — instead of an opaque object with an internal `_linearized_value` member: a **natvis** for the
Visual Studio (MSVC) debugger, and an **LLDB** formatter for LLDB and its front-ends (CLion, Xcode, the
VS Code CodeLLDB extension). The visualizer changes only the debugger display, never the program's
behavior.*

## What you get

Without the visualizer, a `meters<double>` in the Watch or Locals window shows as a struct with a
floating-point member. With it, the same quantity shows as its natural rendering:

```
distance   5 m        meters<double>
elapsed    3 s        seconds<double>
```

Both visualizers are general by design: a single set of wildcard rules covers every unit type — length,
time, frequency, compound, dimensionless, decibel — with no unit name enumerated anywhere. A named unit
(a class deriving from `units::unit<...>`) is rendered through its base, so `meters<double>` shows as
`5 m` too.

## Visual Studio (natvis)

The file [`natvis/units.natvis`](../../natvis/units.natvis) is attached to the `units` CMake interface
target and installed with the package. When you link `units::units` (see
[CMake integration](cmake-integration.md)) under MSVC, Visual Studio picks it up automatically for that
target — no project setting, no manual "add existing item."

If you consume the headers without CMake, add the file to your solution manually: place `units.natvis`
in a location Visual Studio scans (for example `%USERPROFILE%\Documents\Visual Studio 20xx\Visualizers\`)
or add it to your project.

## LLDB (and CLion / Xcode / CodeLLDB)

The file [`natvis/units_lldb.py`](../../natvis/units_lldb.py) is an LLDB summary provider. Load it into a
debug session, or once from your `~/.lldbinit`:

```
command script import /path/to/units/natvis/units_lldb.py
```

A quantity then summarizes as its value and abbreviation:

```
(lldb) frame variable distance
(units::length::meters<double>) distance = 5 m
```

The abbreviation (`m`, `s`, `Hz`, …) is read by evaluating the unit's `abbreviation()` on a live process;
where evaluation is unavailable (a core file, a heavily optimized build) the summary falls back to the
conversion-factor tag name in brackets, so it is always present and readable. Front-ends built on LLDB —
CLion, Xcode, and the VS Code CodeLLDB extension — pick up the same formatter; point their "LLDB init"
setting at the `command script import` line above.

> **Note — the visualizers change only the *debugger* display, never program behavior.** At run time the
> ordinary `operator<<` rendering (`std::cout << distance;` → `5 m`) serves the same purpose on every
> compiler.
