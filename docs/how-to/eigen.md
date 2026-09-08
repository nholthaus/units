# Eigen interoperability

Store dimensioned quantities in [Eigen](https://eigen.tuxfamily.org) vectors and matrices, with the dimensions
checked at compile time.

`units` and Eigen compose without either library depending on the other. Include `<units/eigen.h>` (it is also
pulled in automatically by `<units.h>`) and, if `<Eigen/Core>` is on your include path, `units` teaches Eigen how
to use a unit as a matrix scalar. If Eigen is not present, the header is a no-op — `units` has no dependency on
Eigen, exactly as its optional JSON support has no dependency on a JSON library. There is nothing to enable and
no build flag to set; the support activates from the presence of the Eigen headers alone.

```cpp
#include <Eigen/Core>
#include <units.h>          // brings in units/eigen.h; activates because <Eigen/Core> is available

using namespace units;
using namespace units::literals;

Eigen::Matrix<meters<double>, 3, 1> position;
position << 1.0_m, 2.0_m, 3.0_m;   // a 3-vector of lengths
```

## What works directly on Eigen expressions

Once a unit is a valid scalar, the operations whose result keeps the same dimension work on Eigen vectors and
matrices with no special syntax:

| Operation | Example | Result |
|---|---|---|
| Construction / storage | `Eigen::Matrix<meters<double>, 3, 1> v;` | a vector of `meters` |
| Element access | `v(0)`, `v.x()` | a `meters<double>` |
| Addition / subtraction | `a + b`, `a - b` | a vector of `meters` |
| Scale by a plain scalar | `v * 2.0`, `2.0 * v`, `v / 2.0` | a vector of `meters`. Available wherever the same operation on one coefficient is, so a vector of `celsius` scales in its coefficients' own scale, while a vector of `dBW` does not scale at all — the scalar `dBW * 2.0` does not exist either |
| Reductions that keep the dimension | `v.sum()` | a `meters<double>` |
| Block / segment views | `v.head<2>()` | a vector of `meters` |
| `Map` over unit storage | `Eigen::Map<Vector3m>(ptr)` | a view of `meters` |
| Cast the underlying type | `vi.cast<meters<double>>()` | `meters<int>` → `meters<double>` |

```cpp
Eigen::Matrix<meters<double>, 3, 1> a, b;
a << 1.0_m, 2.0_m, 3.0_m;
b << 10.0_m, 20.0_m, 30.0_m;

auto           sum   = a + b;       // Matrix<meters<double>,3,1>
auto           twice = a * 2.0;     // scaled, still meters
meters<double> total = a.sum();     // 6 m
```

## Operations whose result changes dimension

A dot product of two lengths is an **area**; a norm takes a square root; a cross product of two lengths is an
area vector. Eigen's built-in `dot()`, `norm()`, and `cross()` assume that the product of two scalars is the same
scalar type, which is not true for dimensioned quantities — so `units` provides free helper functions that return
the dimensionally-correct type:

| Helper | Meaning | Result dimension |
|---|---|---|
| `unit_dot(a, b)` | dot product Σ aᵢ·bᵢ | product of the operands' units (meters · meters → square_meters) |
| `unit_squared_norm(v)` | `unit_dot(v, v)` | the squared unit |
| `unit_norm(v)` | Euclidean magnitude √(Σ vᵢ²) | the vector's own unit |
| `unit_normalized(v)` | direction (unit vector) | dimensionless (a plain-scalar vector) |
| `unit_cross(a, b)` | 3D cross product | product of the operands' units |
| `unit_transform(M, v)` | dimensionless matrix `M` times unit vector `v` | the vector's own unit |

```cpp
#include <units/area.h>

Eigen::Matrix<meters<double>, 3, 1> v;
v << 3.0_m, 4.0_m, 0.0_m;

auto           dot  = unit_dot(v, v);      // 25 square_meters  (9 + 16)
meters<double> len  = unit_norm(v);        // 5 m
auto           dir  = unit_normalized(v);  // (0.6, 0.8, 0.0), dimensionless

static_assert(traits::is_area_unit_v<decltype(dot)>);
```

### Rotations and direction-cosine matrices

The common aerospace / simulation case is a dimensionless rotation (or direction-cosine) matrix applied to a
dimensioned position, velocity, or acceleration. `unit_transform` applies a plain-scalar matrix to a vector of
units and returns a vector of the same unit:

```cpp
Eigen::Matrix<meters<double>, 3, 1> position;
position << 1.0_m, 0.0_m, 0.0_m;

Eigen::Matrix<double, 3, 3> rotation;      // 90-degree rotation about z
rotation << 0.0, -1.0, 0.0,
            1.0,  0.0, 0.0,
            0.0,  0.0, 1.0;

Eigen::Matrix<meters<double>, 3, 1> rotated = unit_transform(rotation, position);
// rotated == (0 m, 1 m, 0 m); the magnitude is preserved because the rotation is orthonormal
```

The cross product carries the product dimension — a moment arm (`meters`) crossed with a force (`newtons`) is a
torque-dimensioned (`newton_meters`) vector, for example:

```cpp
Eigen::Matrix<meters<double>, 3, 1> arm;
Eigen::Matrix<newtons<double>, 3, 1> force;
// ...
auto moment = unit_cross(arm, force);   // a vector in newton_meters
```

## Notes and caveats

- **A matrix's scalar is a single type.** Eigen requires one scalar type per matrix, so a matrix cannot mix
  dimensions across its entries. Model a mixed-dimension state vector (position + velocity) as separate typed
  vectors, or store the raw underlying values and attach units at the boundary.
- **The transform matrix is dimensionless.** `unit_transform` takes a plain-scalar (e.g. `double`) matrix. A
  transform that itself scales dimension (for instance an integration step that multiplies a velocity by a time)
  is expressed by combining typed vectors with `unit_dot`/`unit_cross`, or by attaching the unit after the
  numeric product.
- **`scalar / unit` is not enabled.** Dividing a unit vector by a plain scalar (`v / 2.0`) works and keeps the
  dimension; the reverse (a scalar divided elementwise by a unit) would produce a reciprocal dimension and is
  rarely intended, so it is deliberately not provided.
- **Integer underlying types.** A `Matrix<meters<int>, …>` is a valid Eigen matrix; the same lossless-conversion
  rules that apply to a scalar `meters<int>` apply to its entries.

## How it works

The header specializes two Eigen traits for any type satisfying the `units::UnitType` concept:
`Eigen::NumTraits` (so Eigen knows the scalar's numeric properties, forwarded from the underlying arithmetic
type) and `Eigen::ScalarBinaryOpTraits` for the product and quotient with a plain scalar (so scaling an ordinary unit by a
scalar yields the same unit). Because a named unit such as `meters<double>` is a class derived from
`units::unit`, the specializations are written against the concept, not a structural `unit<...>` pattern. The
dimension-changing operations cannot be expressed through Eigen's scalar-preserving assumption, which is why they
are free helper functions rather than trait specializations. No part of `units` is modified to support Eigen; the
whole adapter lives in `units/eigen.h`, guarded by `__has_include(<Eigen/Core>)`.
