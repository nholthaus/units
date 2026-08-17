// ODR-AUDIT REPRO s06 -- SOURCE: operator/ (m/s) x SURFACE: sizeof/alignof/trivial-copy
// Compares the NAMED velocity type against the plain unit<...> base for the SAME m/s
// result. If these ever differ, the ODR violation would also be a LAYOUT mismatch
// (miscompile-grade). For m/s they are IDENTICAL (same size/align/trivially-copyable),
// so the m/s divergence is TYPE-IDENTITY-only at the ABI layer -- still UB, but not a
// layout mismatch. (Other results are checked by the multiply/pow/sqrt slice.)
#include <units/velocity.h>
#include <units/length.h>
#include <units/time.h>
#include <type_traits>
#include <cstdio>
int main(){
  using Named = units::velocity::meters_per_second<double>;
  using Plain = units::unit<units::conversion_factor<std::ratio<1>,
      units::dimension_t<units::dim<units::dimension::length_tag, std::ratio<1>>,
                         units::dim<units::dimension::time_tag, std::ratio<-1>>>>,
      double, units::linear_scale>;
  std::printf("Named: size=%zu align=%zu trivially_copyable=%d trivial=%d standard_layout=%d\n",
    sizeof(Named), alignof(Named), (int)std::is_trivially_copyable_v<Named>, (int)std::is_trivial_v<Named>, (int)std::is_standard_layout_v<Named>);
  std::printf("Plain: size=%zu align=%zu trivially_copyable=%d trivial=%d standard_layout=%d\n",
    sizeof(Plain), alignof(Plain), (int)std::is_trivially_copyable_v<Plain>, (int)std::is_trivial_v<Plain>, (int)std::is_standard_layout_v<Plain>);
  const bool same_size = (sizeof(Named)==sizeof(Plain));
  const bool same_align = (alignof(Named)==alignof(Plain));
  const bool same_triv = ((int)std::is_trivially_copyable_v<Named> == (int)std::is_trivially_copyable_v<Plain>);
  std::printf("SAME size=%d align=%d triv_copy=%d\n", (int)same_size, (int)same_align, (int)same_triv);
}
