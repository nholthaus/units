CORRECTNESS-AFFECTING ODR (not cosmetic): overload dispatch on a named result type.

A user provides a generic handle<U>() and a more-specialized handle(meters_per_second<double>)
(a normal thing to do — "speeds get special handling"). A weak inline compute() calls
handle(meters<double>(5)/seconds<double>(2)). Whether the NAMED overload is selected depends on
whether the TU saw <units/velocity.h> (both because the result TYPE is named-vs-raw AND because the
overload is visible). compute() therefore has two definitions; the linker keeps one by LINK ORDER,
and the returned NUMBER flips 102.5 <-> 202.5 across the whole program.

Build (from this dir), both link orders:
  g++ -std=c++23 -I ../../../include -c corr_dispatch_a.cpp corr_dispatch_b.cpp corr_dispatch_main.cpp
  g++ corr_dispatch_main.o corr_dispatch_a.o corr_dispatch_b.o -o p1 && ./p1   # -> 102.5
  g++ corr_dispatch_main.o corr_dispatch_b.o corr_dispatch_a.o -o p2 && ./p2   # -> 202.5
EXPECTED: a==b and stable regardless of link order.
OBSERVED: from_a()==from_b() within a run, but BOTH flip 102.5<->202.5 by link order — a silent,
value-level miscompilation (from_b() runs an overload its TU never included).
