Thanks for the report. Digging into this, the conclusion is a bit different from the proposed fix, so let me lay out the reasoning.

`operator%` on unit types is an **integer count operation**, not a physical one. It operates on the operands' point values (their `.raw()` counts), and the built-in `%` it delegates to is integer-only — `double % double` is not a valid expression in C++, and `fmod` is the floating-point remainder. So `%` is only meaningful between operands whose point counts are *in the same tick scale*.

Under that model, the example `percent<int>{50} % parts_per_million<int>{300000}` is not "50% mod 300000ppm = 20%" — it is a modulo of two counts measured in *different ticks* (percent-ticks vs ppm-ticks). Neither `50 % 300000 = 50` nor the physical `0.5 % 0.3 = 0.2` is a well-defined answer, because the operation itself has no meaning when the operands don't share a tick. Converting to a common scale first (as `+`/`-`/`fmod` do) would make it *look* meaningful, but `%` is a count operation, and silently rescaling counts to force a number is the wrong fix.

So rather than change the result, we now **reject the mixed-scale case at compile time**. As of the linked change:

- `percent{n} % percent{m}` — same tick scale, well-defined. Compiles. ✅
- `percent{n} % dimensionless{m}` and `percent{n} % m` (a bare integer) — a ratio unit modulo a plain count. Compiles. ✅
- `percent{n} % parts_per_million{m}` — two *different* ratio-scaled dimensionless units, no shared tick. **Does not compile** (a deleted overload gives a clear diagnostic). ✅
- Any floating-point operand (`percent<double> % percent<double>`, `meters<double> % meters<double>`, …) — `%` is integer-only; **does not compile**. Use `fmod` for a floating-point remainder. ✅

This matches how the library already treats ratio-scaled dimensionless mixing elsewhere (the compound-assignment operators already excluded it), and it is consistent with the standard's rule that `%` is integral.

If you want a remainder on the *physical values* (`0.5 fmod 0.3 = 0.2`), that is exactly what `fmod` is for, and it works across scales: `fmod(percent<double>{50}, parts_per_million<double>{300000})` gives `0.2`.

We considered auto-promoting a floating operand to an integer to keep `%` working for it, but that would silently truncate (`50.7% → 50%`) — the kind of lossy surprise the library deliberately rejects — so `fmod` is the right tool for the floating case rather than a promotion.

A full compile-time contract test (every accepted and rejected operand pairing for both `%` and `%=`) now locks this behavior.
