# Visual Studio debugger visualizer

*`units` ships a natvis visualizer so that, when debugging in Visual Studio, a quantity displays as its
value and abbreviation — `5 m`, `3 s` — instead of an opaque object with an internal `_linearized_value`
member. It attaches automatically; there is nothing to configure.*

## What you get

Without the visualizer, a `meters<double>` in the Watch or Locals window shows as a struct with a
floating-point member. With it, the same quantity shows as its natural rendering:

```
distance   5 m        meters<double>
elapsed    3 s        seconds<double>
```

## How it attaches

The file [`natvis/units.natvis`](../../natvis/units.natvis) is attached to the `units` CMake interface
target and installed with the package. When you link `units::units` (see
[CMake integration](cmake-integration.md)) under MSVC, Visual Studio picks it up automatically for that
target — no project setting, no manual "add existing item."

If you consume the headers without CMake, add the file to your solution manually: place `units.natvis`
in a location Visual Studio scans (for example `%USERPROFILE%\Documents\Visual Studio 20xx\Visualizers\`)
or add it to your project.

> **Note — MSVC only.** natvis is a Visual Studio debugger feature. It has no effect with GCC or Clang;
> for those, the ordinary `operator<<`/`to_string` rendering (`std::cout << distance;` → `5 m`) serves the
> same purpose at run time. The visualizer changes only the *debugger* display, never the program's
> behavior.
