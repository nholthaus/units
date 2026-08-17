Chip — good report, thank you! I hadn't caught it before.

The observable effects are mostly cosmetic — the same value printing as `5 mps` in one build and `5 m s^-1`. The sharp case is dispatch on the named type. I found that if one header provides a specialized overload for the named result:

```cpp
// generic, always available
template<class U> double handle(U v)                        { return 200.0 + v.value(); }
// specialized for named speeds, only where <units/velocity.h> is included
double handle(units::velocity::meters_per_second<double> v) { return 100.0 + v.value(); }

inline double compute() { return handle(units::meters<double>(5) / units::seconds<double>(2)); }
```

then `compute()` is `102.5` in a TU that saw the velocity header and `202.5` in one that didn't, and since it's a weak inline the linker keeps one for the whole program — a silent numeric flip decided by link order. Same shape for a `std::hash` or `std::formatter` specialized on the named type: it applies in one TU and is missed in another.

Here's how I'm thinking about it: I've designed this library around type *equivalence*, not type *equality* — two spellings that carry the same value and the same dimension denote the same quantity and are interchangeable, whether or not they're the identical C++ type. A quantity *is* its value and its dimensions; "correctness" means the dimension is right and the value is right, not that the type is byte-identical across TUs. That's my dimension-only model (lacking any concept of kinds for now). The named-type identity is then the only thing that can diverge and the sharp cases are exactly the ones that dispatch on it. The two divergent forms are *equivalent* by that design; the flip only appears when user code asks for *equality* — a specialization or overload keyed on the exact type — which is a stricter thing than the library sets out to guarantee.

And that identity is the only thing that diverges as far as I can reproduce. I think I had the README too strict about including resultant types - it's a best practice for sure, but you won't get the wrong arithmetic if you don't. A named unit adds no data members over its base, so both forms are the same size and layout and the value is never corrupted. Serialization is keyed on the dimension, not the named type, so both forms serialize identically. And a function that constrains on a quantity can only be written in a TU that already sees the named type, so it never gets handed a plain-base value it wasn't expecting. In the flip above, the value and the dimension going into `handle` are identical in both TUs; only the type tag it dispatches on differs, and that tag isn't something this model assigns meaning to - for better or worse.

So the print-case I'm just going to leave alone (along with the compiler error messages) - it's best effort, but you get the best names available. For the dispatch case, every dimension now emits a dimension-keyed concept beside its `is_<dimension>_unit` trait, so you constrain on the dimension instead of the concrete type and the result classifies the same in every TU:

```cpp
void handle(units::Velocity auto v);   // dimension-keyed; cannot flip by link order

template <units::Force F>
F clamp_force(F f);
```

Written that way the overload won't flip. And as a best practice, include the dimension your results land in (`<units/velocity.h>` when you name a `meters_per_second`) so result types name consistently and diagnostics stay readable.

I'm reluctant to reach for the obvious fixes — collapsing named results back to a plain type or a full global include — because the named class is what makes a diagnostic read `meters<double>` instead of a page of `unit<conversion_factor<...>>`, and that readability is the feature people lean on most, and global includes bloat the compile time unsustainably. So: it's genuine UB and I'm not writing it off, but the value and the dimension are always intact, the only behavioral case I could produce is the named-type dispatch above, and the concepts plus the header guidance address it directly. On that balance I'll keep it as-is for now, and revisit if a stronger counterexample turns up — something that corrupts a value or a dimension, or a real case the concepts don't cover. Bisecting the repo, I think this UB has been present since at least 3.1, which I've used for 3-4 years and (thankfully) haven't encountered this issue in the wild.

Thanks again — this was helpful and I'll be making both code and doc changes to help sharpen the point.

— Nic
