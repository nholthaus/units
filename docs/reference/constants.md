# Physical constants

*The `units::constants` namespace provides the physical constants as typed quantities — each carries its
correct dimension, so it participates in dimensional analysis like any other quantity. Values are the
2018 CODATA recommended values. Defined in [`include/units.h`](../../include/units.h).*

```cpp
#include <units.h>
using namespace units;

// The constants are typed quantities, so this is dimensionally checked:
auto momentum = constants::m_e * (0.01 * constants::c);   // electron mass * a velocity -> momentum
```

Bring them into scope with `units::constants::` (or `using namespace units;` and `constants::c`).

| Symbol | Constant | Value | Dimension |
|--------|----------|-------|-----------|
| `pi`        | Ratio of a circle's circumference to its diameter | 3.14159265358979323846 | dimensionless |
| `c`         | Speed of light in vacuum          | 299 792 458 | m/s |
| `G`         | Newtonian constant of gravitation | 6.67430×10⁻¹¹ | m³·kg⁻¹·s⁻² |
| `h`         | Planck constant                   | 6.62607015×10⁻³⁴ | J·s |
| `h_bar`     | Reduced Planck constant (ℏ)       | 1.054571817×10⁻³⁴ | J·s |
| `mu0`       | Vacuum permeability (μ₀)          | 1.25663706212×10⁻⁶ | N·A⁻² |
| `epsilon0`  | Vacuum permittivity (ε₀)          | 8.8541878128×10⁻¹² | F·m⁻¹ |
| `Z0`        | Characteristic impedance of vacuum | 376.730313668 | Ω |
| `k_e`       | Coulomb constant                  | 8.9875517923×10⁹ | N·m²·C⁻² |
| `e`         | Elementary charge                 | 1.602176634×10⁻¹⁹ | C |
| `m_e`       | Electron mass                     | 9.1093837015×10⁻³¹ | kg |
| `m_p`       | Proton mass                       | 1.67262192369×10⁻²⁷ | kg |
| `mu_B`      | Bohr magneton                     | 9.2740100783×10⁻²⁴ | J·T⁻¹ |
| `N_A`       | Avogadro constant                 | 6.02214076×10²³ | mol⁻¹ |
| `R`         | Molar gas constant                | 8.314462618 | J·K⁻¹·mol⁻¹ |
| `k_B`       | Boltzmann constant                | 1.380649×10⁻²³ | J·K⁻¹ |
| `F`         | Faraday constant                  | 96 485.33212 | C·mol⁻¹ |
| `sigma`     | Stefan–Boltzmann constant (σ)     | 5.670374419×10⁻⁸ | W·m⁻²·K⁻⁴ |

Because each constant is a typed quantity, an expression that misuses one — for example adding `c` (a
velocity) to `m_e` (a mass) — is a compile error, just like any other dimensional mistake (see
[type safety](../explain/type-safety.md)).
