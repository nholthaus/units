# Serialization

Encode a quantity to a self-describing byte stream, and decode it on the other side without prior agreement
on its type.

`<units/serialization.h>` writes a quantity to a compact binary stream that carries both the value and the
dimension. A reader recovers the quantity from the bytes alone: it discovers *what dimension* the stream holds
before it names a target type, so the two peers need no shared header, no schema, and no out-of-band agreement
on the unit. The encoding is not limited to the built-in dimensions — any base dimension, including one you
define yourself, round-trips.

This is a separate, opt-in header. It is not pulled in by `<units.h>`; include it only where you serialize.

```cpp
#include <units.h>
#include <units/serialization.h>
```

Related how-to guides: [JSON serialization](json-serialization.md), [defining new units](defining-new-units.md),
[chrono interop](chrono-interop.md).

## Round-trip

Write a quantity to a stream and read it back. `serialize` gives you the bytes to write; on the way back
`deserialize` hands you the quantity, or a `deserialize_error`.

```cpp
#include <units.h>
#include <units/serialization.h>
#include <fstream>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

    // write straight to a stream — a file, a socket, any std::ostream
    {
        std::ofstream out("speed.bin", std::ios::binary);
        const auto     speed = serialize(60.0_mph);
        out.write(speed.data(), speed.size());        // const char* + size, no cast
    }

    // read it back, knowing nothing about the type in advance
    std::ifstream          in("speed.bin", std::ios::binary | std::ios::ate);
    std::vector<std::byte> bytes(in.tellg());
    in.seekg(0);
    in.read(reinterpret_cast<char*>(bytes.data()), bytes.size());

    auto decoded = deserialize(bytes);                // decode
    if (!decoded)
        return 1;

    auto kph = decoded->to<kilometers_per_hour<double>>();   // collapse to a concrete unit, checked
    if (kph)
        std::cout << kph->value() << " kph\n";               // prints: 96.5606 kph
}
```

The stream is self-describing: the reader was never told the value was a velocity. It read the dimension out of
the bytes, and `to<kilometers_per_hour<double>>()` succeeded because that dimension matched. Ask for a dimension
the stream does not hold and the collapse fails cleanly rather than misreading the number.

## `any_unit` — the erased quantity, a first-class value

Both `serialize` and `deserialize` center on `any_unit`: `serialize(q)` returns one (owning the encoded bytes),
and `deserialize` returns one (wrapped in `std::expected`, since malformed bytes can fail). It is a value type,
not a bag of bytes — it compares, orders within a dimension, hashes, and prints.

**Its bytes, two ways.** `any_unit` owns the serialized form; you view it through a type-safe span or a
C-interface pair, both valid for the object's lifetime:

- `bytes()` → `std::span<const std::byte>` — the modern view; feed it straight back to `deserialize`.
- `data()` → `const char*` and `size()` → `std::size_t` — for the byte-oriented interfaces that take a
  `const char*`/`const void*` and a length. They drop into `std::ostream::write`, `std::fwrite`, and a socket
  `send` with **no cast at the call site** (the one `reinterpret_cast` lives inside `data()`).

```cpp
any_unit q = serialize(60.0_mph);

file.write(q.data(), q.size());                 // std::ostream::write(const char*, n) — no cast
std::fwrite(q.data(), 1, q.size(), fp);         // C stdio
::send(sock, q.data(), q.size(), 0);            // const char* decays to const void*

for (std::byte b : q.bytes()) { /* type-safe iteration */ }
auto same = deserialize(q.bytes());             // span round-trips
```

> **Caveat — the byte views are non-owning.** `bytes()`, `data()`, and `size()` view the buffer *inside* the
> `any_unit`; they are valid only while that `any_unit` is alive. Keep it in a named variable rather than calling
> `serialize(q).bytes()` on a temporary. To hold the bytes past the `any_unit`, copy them into your own
> `std::vector<std::byte>`.

**Comparison, ordering, hashing, printing.** `any_unit` behaves like the value it represents:

```cpp
serialize(meters<double>(1000.0)) == serialize(kilometers<double>(1.0));   // true — same dimension AND base value
serialize(meters<double>(3.0))    <  serialize(meters<double>(5.0));       // true — ordered within a dimension
serialize(meters<double>(3.0))    <  serialize(seconds<double>(3.0));      // false — different dimensions are UNORDERED

std::unordered_map<any_unit, Job> byQuantity;   // std::hash<any_unit> — a usable key
std::cout << serialize(meters<double>(100.0));   // text form: base value + hashed dimension signature
```

Equality is *same dimension and same SI-base magnitude* (using the same relative tolerance as the concrete
`unit` comparison), so it is a comparison of quantities, not of unit names or of bytes. Ordering is a
`std::partial_ordering`: quantities of one dimension order by magnitude, and quantities of different dimensions
are unordered — so `<`/`<=`/`>`/`>=` are all false between them (and `any_unit` is therefore an
`unordered_map`/`unordered_set` key, not a `std::set` key across mixed dimensions). Because the erased form
carries each base dimension by *name hash*, not name, `operator<<` prints the SI-base magnitude and the hashed
signature (`#<hash>^<exponent>`); to print with unit names, collapse to a concrete unit first and stream that.

## Collapsing an erased quantity

Beyond the value-type surface above, these bring an `any_unit` down to a concrete typed quantity.

**`to<Unit>()` — checked, the safe default.** Returns `std::expected<Unit, deserialize_error>`; a dimension
mismatch is a value, not an exception.

```cpp
auto v = decoded->to<meters_per_second<double>>();      // std::expected
if (v)
    use(*v);

auto wrong = decoded->to<meters<double>>();             // a velocity is not a length
// wrong.error() == deserialize_error::dimension_mismatch
```

**`try_to<Unit>()` — throwing.** Same collapse, but throws `std::runtime_error` on a mismatch. Use it where the
type is known and a mismatch is a programming error.

```cpp
meters_per_second<double> v = decoded->try_to<meters_per_second<double>>();
```

**`unit_cast<Unit>(v)` — throwing free function.** The free-function spelling of `try_to`, mirroring
`std::any_cast`.

```cpp
auto v = units::unit_cast<meters_per_second<double>>(*decoded);
```

**`visit(f)` — no target named.** Invokes a generic callable with the *canonical* SI unit of whichever dimension
the stream holds. You name no target type at the call site, yet every operation inside the visitor is still
dimensionally checked at compile time. This is the tool when a stream may carry any of several dimensions and you
want to branch on what arrived.

```cpp
decoded->visit([](auto q) {
    // q is the canonical SI unit of the decoded dimension (here, 26.8224 m/s for 60 mph)
    std::cout << q.value() << " (SI base)\n";
});
```

With no explicit candidates, every dimension the library defines is a candidate, so velocity, force, energy, and
the rest resolve out of the box. Pass explicit candidates to resolve a user-defined dimension, to restrict the
set the visitor must handle, or to disambiguate two dimensions that share a signature (torque and energy have the
same base terms; the first candidate listed wins):

```cpp
decoded->visit<dimension::energy, dimension::torque>([](auto q) { /* ... */ });
```

A `visit` with no matching candidate throws `std::runtime_error`.

**The visitor body must compile for every candidate.** `visit` instantiates the callable once per candidate
dimension — with no explicit candidates, that is *every* dimension the library defines. So the body must be valid
for any quantity: use the generic quantity API (`value()`, arithmetic, comparison, printing), not an assignment or
a call that names one specific unit type. Naming a unit forces the body to make sense for length, mass, energy,
and the rest all at once, which does not compile:

```cpp
// Ill-formed with no explicit candidates: this body must also compile for the length candidate,
// where q is a length and cannot become a velocity.
// decoded->visit([](auto q) { meters_per_second<double> speed = q; });
```

Restrict the candidate set to make the body specific. When you list exactly the dimensions you expect, the
visitor is instantiated only for those, so it may name their units:

```cpp
decoded->visit<dimension::velocity>([](auto q) {
    meters_per_second<double> speed = q;    // q is velocity's canonical unit; this compiles
    std::cout << speed.value() << " m/s\n";
});
```

## The typed fast path

When the type *is* known ahead of time, `deserialize<Unit>(bytes)` decodes straight into it, skipping the erased
intermediate. It is `deserialize` followed by `to<Unit>()`, in one call.

```cpp
auto v = units::deserialize<miles_per_hour<double>>(bytes);   // std::expected<miles_per_hour<double>, ...>
if (v)
    std::cout << v->value() << " mph\n";                      // prints: 60 mph
```

The stream is still self-describing — a dimension mismatch here surfaces as
`deserialize_error::dimension_mismatch` rather than a misread value.

## The error model

Every fallible entry point returns `std::expected<..., deserialize_error>` (the throwing collapses convert the
error into a `std::runtime_error`). The reasons are exhaustive:

| `deserialize_error` | Meaning |
|---|---|
| `truncated` | the byte range ended before a complete quantity was read |
| `bad_version` | the stream's format-version byte is not one this build understands |
| `dimension_mismatch` | the stream's dimension does not match the requested target |
| `unknown_base_dimension` | reserved: a base-dimension code this build does not know |
| `lossy_target` | the value cannot be represented in the requested underlying type without loss |

An unknown base-dimension hash is not itself an error at decode time — the stream decodes into an `any_unit`
carrying that dimension, and it surfaces as `dimension_mismatch` when you try to collapse it into a target whose
signature it cannot match. The `unknown_base_dimension` code is part of the public enum for exhaustive `switch`
coverage; the current decoder does not produce it.

```cpp
using namespace units;

std::vector<std::byte> empty;
auto a = deserialize(empty);
// !a && a.error() == deserialize_error::truncated

auto bytes = serialize(0.5_m);
auto d     = deserialize(bytes);
auto lossy = d->to<meters<int>>();     // 0.5 has no exact int representation
// !lossy && lossy.error() == deserialize_error::lossy_target
```

`lossy_target` is the same guard the rest of the library applies to a lossy narrowing: collapsing a fractional
value into an integer underlying type is an error, not a silent truncation.

## Extensibility: any dimension, including your own

The stream identifies each base dimension by an 8-byte FNV-1a hash of the dimension's `name` string, not by a
position in a fixed table. Two consequences follow. There is no fixed set of dimensions the format can represent
— a dimension the library has never seen still encodes and decodes. And there is no ceiling on how many base
dimensions one quantity may compose; the signature is a variable-length list.

A base dimension you define with `make_dimension` serializes and round-trips with no registration anywhere:

```cpp
#include <units.h>
#include <units/serialization.h>
#include <iostream>

namespace units::dimension
{
    struct sparkliness_tag
    {
        static constexpr auto name         = "sparkliness";
        static constexpr auto abbreviation = "spk";
    };
    using sparkliness = make_dimension<sparkliness_tag>;
}

namespace units
{
    UNIT_ADD(sparkliness, sparkles, spk, conversion_factor<std::ratio<1>, dimension::sparkliness>)
    UNIT_ADD_DIMENSION_TRAIT(sparkliness)
}

int main()
{
    using namespace units;
    using namespace units::literals;

    auto bytes   = serialize(42.0_spk);          // a dimension the library never defined
    auto decoded = deserialize(bytes);

    auto back = decoded->to<sparkles<double>>();  // the erased path knows nothing library-specific
    if (back)
        std::cout << back->value() << " spk\n";   // prints: 42 spk
}
```

The `to<Unit>()`, `try_to<Unit>()`, and `unit_cast` paths work for a user-defined dimension exactly as for a
built-in one, because they compare the decoded signature against a *named* target type you supply.

`visit`, though, must be *told* the user dimension:

```cpp
decoded->visit<dimension::sparkliness>([](auto q) {
    std::cout << q.value() << " (canonical spk)\n";
});
```

This is a fundamental limit, not an omission. `visit` resolves a runtime hash into a C++ type by trying its
candidate list; with no explicit candidates it tries the library's own dimensions. C++ cannot materialize a type
from a runtime value, so a dimension the compiler was never shown at the call site cannot be a candidate — the
runtime-to-type wall. List your dimension as a candidate and the wall is gone.

## The wire format

The stream is a version byte, a one-byte header, a variable-length dimension signature, and the value in SI
canonical base. Every integer is an LEB128 varint (zig-zag for signed values), so small magnitudes and small
exponents cost few bytes.

```
+---------+--------+------------------+----------------------------------+-------------------+
| version | header |  term count (v)  |  N x { hash[8] | num(v) [den(v)] }|  value            |
+---------+--------+------------------+----------------------------------+-------------------+
   1 byte   1 byte    varint            8 bytes + varint(s) per term         varint | 4 | 8
```

- **version** — one byte, the format version. A byte a build does not recognize is rejected as `bad_version`.
- **header** — one byte, bit-packed: the low two bits are the value's encoding kind (integer varint, 32-bit
  float, or 64-bit double); one bit flags whether any dimension term carries a fractional exponent. The rest is
  reserved.
- **term count** — a varint: the number of *nonzero* base-dimension terms. A dimensionless quantity has zero
  terms; a velocity has two (length and inverse time). This is why the format has no fixed dimension ceiling —
  the signature is a list, not a fixed-width mask.
- **each term** — the base dimension's 8-byte name-hash, then the exponent numerator as a signed varint. The
  denominator follows (also a signed varint) *only* when the header's fractional-exponent flag is set, so the
  common integer-exponent case pays nothing for it. Terms are sorted by hash, so signature equality is
  order-independent.
- **value** — the magnitude in SI canonical base, in the tersest *exact* encoding the header selected:
  - an integer value in base is a zig-zag varint (an integer number of meters, bytes, amperes costs only as many
    bytes as its magnitude needs);
  - a value that is exactly representable as a 32-bit float is four bytes;
  - anything else is the full eight-byte double.

The value is always stored in SI canonical base, so the stream is unit-agnostic within its dimension: `60_mph`
and `26.8224_mps` serialize to the same value bytes, and either decodes to whichever velocity unit the reader
asks for.

## Performance

The numbers below are measured on the machine that built the docs (GCC 15, `x86-64`); treat them as
representative, not as guarantees. Reproduce them with the snippets under
[`examples/`](../../examples/) compiled at `-std=c++23 -I include`.

### Size

Bytes per serialized quantity across a spread, next to a naive `{"value":V,"unit":"U"}` JSON string for the same
quantity. The binary stream is self-describing where the JSON is not — the JSON needs both peers to agree on the
unit out of band, whereas the binary carries the dimension.

| Quantity | Serialized bytes | Naive JSON string |
|---|---:|---:|
| `100.0_m` (integer meters) | 14 | 24 |
| `5000.0_g` (5 kg) | 13 | 23 |
| `1.0_GB` | 17 | 23 |
| `100.0_TB` | 19 | 25 |
| `2.5_A` | 16 | — |
| `1.5_V` | 43 | — |
| `20.0_degC` | 20 | 26 |
| `100.0_psi` | 38 | 26 |
| `60.0_mph` | 29 | 25 |
| `9.81_mps2` | 29 | 29 |
| `dimensionless<double>(0.25)` | 7 | — |
| `42.0_spk` (user-defined dimension) | 13 | — |

Size tracks the two things that vary: how many base-dimension terms the quantity has (each is an 8-byte hash plus
its exponent), and whether the SI-base value lands as an integer varint, a 32-bit float, or a full double. A
single-term integer quantity is small; a compound quantity whose base value is an irrational double
(`1.5_V` decomposes into four base dimensions with a 64-bit value) is larger. The self-describing dimension is
carried in the hash-keyed terms rather than a unit string, so it stays compact even for units with long names.

### Compile time

Best-of-three warmed incremental builds of a single translation unit, measured with a Python subprocess timer.
These are *warm* numbers — the compiler and filesystem caches are hot — and are machine-dependent; absolute
values will differ on your box, but the deltas are the point.

| Translation unit | Compile time | Delta |
|---|---:|---:|
| (a) `#include <units.h>` only | ~4.4 s | baseline |
| (b) + `<units/serialization.h>`, `serialize` + `deserialize` + `to` | ~4.9 s | +0.4 to +0.6 s |
| (c) + a `visit()` call | ~5.1 s | +0.14 to +0.18 s over (b) |

Adding serialization to a translation unit that already includes `<units.h>` costs a small fraction of a second;
`visit()` adds less again. The bulk of the compile time is `<units.h>` instantiating its 48 dimensions, as it is
without serialization — the serialization header adds little on top. If a translation unit's compile time
matters, the larger lever is including only the per-dimension headers you use (see
[subset headers for compile time](subset-headers-compile-time.md)); the serialization header itself is close to
free.

### Run time

A tight loop of `serialize` → `deserialize` → `to<Unit>()` — the full round-trip, including the `serialize`
allocation — runs at roughly **85 ns per round-trip** at `-O2` on the same machine. The dimension compare is over
a handful of 8-byte hashes, and the value codec is a varint or a `memcpy`; the cost is dominated by the buffer
allocation, not the encoding.
