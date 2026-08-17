#include <units/length.h>
#ifdef WITH_AREA
#include <units/area.h>
#endif
#include <typeinfo>
#include <cstdio>
int main(){ using R=decltype(units::pow<2>(units::meters<double>(1)));
  std::printf("pow R=%s\n", typeid(R).name());
  std::printf("pow sizeof=%zu alignof=%zu trivialcopy=%d\n",
      sizeof(R), alignof(R), (int)__is_trivially_copyable(R)); }
